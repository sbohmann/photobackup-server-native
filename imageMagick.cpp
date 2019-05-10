#include <iostream>
#include <cstring>
#include <functional>
#include <sstream>

#include <MagickWand/MagickWand.h>

#include "at_yeoman_photobackup_server_imageMagick_ImageMagick.h"

JNIEXPORT void JNICALL
Java_at_yeoman_photobackup_server_imageMagick_ImageMagick_initialize(JNIEnv *, jclass) {
    MagickWandGenesis();
}

struct PinnedByteArray {
    JNIEnv *const env;
    jbyteArray const array;
    jbyte *const data;
    jsize const length;
    
    PinnedByteArray(JNIEnv *env, jbyteArray array)
            : env(env),
              array(array),
              data(env->GetByteArrayElements(array, 0)),
              length(env->GetArrayLength(array)) {
    }
    
    PinnedByteArray(PinnedByteArray &&other)
            : env(other.env), array(other.array), data(other.data), length(other.length) {
        other.disabled = true;
    }
    
    ~PinnedByteArray() {
        if (!disabled) {
            env->ReleaseByteArrayElements(array, data, 0);
        }
    }

private:
    bool disabled = false;
    
    PinnedByteArray(const PinnedByteArray &);
};

template<typename T>
struct AutoClosing {
    const T instance;
    const std::function<void(const T &)> close;
    
    AutoClosing(const T &entity, const std::function<void(const T &)> close)
            : instance(entity), close(close) {
    }
    
    AutoClosing(AutoClosing &&other)
            : instance(other.instance), close(other.close) {
        other.disabled = true;
    }
    
    ~AutoClosing() {
        if (!disabled) {
            close(instance);
        }
    }
    
    operator const T &() {
        return instance;
    }

private:
    bool disabled = false;
    
    AutoClosing(const AutoClosing &);
};

struct Error : public std::exception {
    const std::string message;
    
    Error(const std::string &message)
            : message(message) {
    }
    
    virtual const char *what() const throw() {
        return message.c_str();
    }
};

auto createWand() {
    AutoClosing<MagickWand *> wand(
            NewMagickWand(),
            [](MagickWand *const instance) {
                if (instance) {
                    DestroyMagickWand(instance);
                }
            });
    
    if (wand.instance == nullptr) {
        throw Error("Unable to create wand");
    }
    
    return wand;
}

void javaThrow(JNIEnv *env, const std::string &message) {
    auto exceptionClass = env->FindClass("java/lang/RuntimeException");
    if (exceptionClass != nullptr) {
        env->ThrowNew(exceptionClass, message.c_str());
    }
}

// TODO handle errors from MagickWand
JNIEXPORT void readImage(AutoClosing<MagickWand *> &wand, PinnedByteArray &pinnedHeicData) {
    if (MagickReadImageBlob(wand, pinnedHeicData.data, (size_t) pinnedHeicData.length) == MagickFalse) {
        ExceptionType exceptionType;
        auto reason = MagickGetException(wand, &exceptionType);
        std::ostringstream message;
        message << "Unable to read input image - " << exceptionType << " - " << reason;
        throw Error(message.str());
    }
}

jbyteArray JNICALL
Java_at_yeoman_photobackup_server_imageMagick_ImageMagick_convertToJpeg(JNIEnv *env, jclass, jbyteArray inputData) {
    try {
        PinnedByteArray pinnedHeicData(env, inputData);
        
        auto wand = createWand();
        
        readImage(wand, pinnedHeicData);
        
        MagickSetImageFormat(wand, "JPEG");
        
        size_t resultBlobSize = 0;
        
        auto resultBlobData = AutoClosing<unsigned char *>(
                MagickGetImageBlob(wand, &resultBlobSize),
                [](unsigned char *instance) {
                    MagickRelinquishMemory(instance);
                });
        
        jbyteArray resultData = env->NewByteArray((jsize) resultBlobSize);
        PinnedByteArray pinnedResultData(env, resultData);
        std::memcpy(pinnedResultData.data, resultBlobData, resultBlobSize);
        
        
        
        return resultData;
    } catch (std::exception &error) {
        javaThrow(env, std::string() + "ImageMagick.convertToJpeg: " + error.what());
        return nullptr;
    } catch (...) {
        javaThrow(env, std::string() + "ImageMagick.convertToJpeg: unknwon error");
        return nullptr;
    }
}

bool dbg = true;

// TODO handle errors from MagickWand
JNIEXPORT jbyteArray JNICALL Java_at_yeoman_photobackup_server_imageMagick_ImageMagick_convertToJpegWithMaximumSize
        (JNIEnv *env, jclass, jbyteArray inputData, jint maximumWidth, jint maximumHeight) {
    
    if (maximumWidth <= 0 || maximumHeight <= 0) {
        std::cerr << "ImageMagick.convertToJpegWithMaximumSize: illegal maximum width / height" <<
                  maximumWidth << " / " << maximumHeight << std::endl;
        return nullptr;
    }
    
    try {
        PinnedByteArray pinnedHeicData(env, inputData);
        
        auto wand = createWand();
    
        readImage(wand, pinnedHeicData);
        
        size_t originalWidth = MagickGetImageWidth(wand);
        size_t originalHeight = MagickGetImageHeight(wand);
        
        double widthRatio = (double) originalWidth / (double) maximumWidth;
        double heightRatio = (double) originalHeight / (double) maximumHeight;
        
        size_t width, height;
        
        if (widthRatio > heightRatio) {
            width = (size_t) maximumWidth;
            height = (size_t) lround((double) originalHeight * (double) maximumWidth / (double) originalWidth);
        } else {
            width = (size_t) lround((double) originalWidth * (double) maximumHeight / (double) originalHeight);
            height = (size_t) maximumHeight;
        }

//        std::cout << "width: " << width << ", height " << height << " for originalWidth " << originalWidth << ", originalHeight " << originalHeight << std::endl;
        
        MagickSetImageFormat(wand, "JPEG");
        
        MagickResizeImage(wand, width, height, BoxFilter);
        
        size_t resultBlobSize = 0;
        
        auto resultBlobData = AutoClosing<unsigned char *>(
                MagickGetImageBlob(wand, &resultBlobSize),
                [](unsigned char *instance) {
                    MagickRelinquishMemory(instance);
                });
        
        jbyteArray resultData = env->NewByteArray((jsize) resultBlobSize);
        PinnedByteArray pinnedResultData(env, resultData);
        std::memcpy(pinnedResultData.data, resultBlobData, resultBlobSize);
    
        if (dbg) return resultData;
        
        MagickRelinquishMemory(resultBlobData);
        
        return resultData;
    } catch (std::exception &error) {
        javaThrow(env, std::string() + "ImageMagick.convertToJpegWithMaximumSize: " + error.what());
        return nullptr;
    } catch (...) {
        javaThrow(env, std::string() + "ImageMagick.convertToJpegWithMaximumSize: unknwon error");
        return nullptr;
    }
}
