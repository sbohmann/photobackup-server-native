#include <iostream>
#include <cstring>
#include <functional>

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
    
    ~PinnedByteArray() {
        env->ReleaseByteArrayElements(array, data, 0);
    }
};

template<typename T>
struct AutoClosing {
    const T instance;
    const std::function<void(const T &)> close;
    
    AutoClosing(const T &entity, const std::function<void(const T &)> &close)
            : instance(entity), close(close) {
    }
    
    ~AutoClosing() {
        close(instance);
    }
    
    operator const T &() {
        return instance;
    }
};

struct Error : public std::exception {
    const std::string message;
    
    Error(const std::string &message)
            : message(message) {
    }
    
    const std::string &what() {
        return message;
    }
};

auto createWand() {
    auto wand = AutoClosing<MagickWand *>(
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

// TODO handle errors from MagickWand
JNIEXPORT jbyteArray JNICALL
Java_at_yeoman_photobackup_server_imageMagick_ImageMagick_convertToJpeg(JNIEnv *env, jclass, jbyteArray inputData) {
    try {
        PinnedByteArray pinnedHeicData(env, inputData);
        
        auto wand = createWand();
        
        MagickReadImageBlob(wand, pinnedHeicData.data, (size_t) pinnedHeicData.length);
        
        MagickSetImageFormat(wand, "JPEG");
        
        size_t resultBlobSize = 0;
        
        unsigned char *resultBlobData = MagickGetImageBlob(wand, &resultBlobSize);
        
        jbyteArray resultData = env->NewByteArray((jsize) resultBlobSize);
        PinnedByteArray pinnedResultData(env, resultData);
        std::memcpy(pinnedResultData.data, resultBlobData, resultBlobSize);
        
        MagickRelinquishMemory(resultBlobData);
        
        return resultData;
    } catch (std::exception &error) {
        std::cerr << "ImageMagick.convertToJpeg: " << error.what() << std::endl;
        return nullptr;
    }
}

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
        
        MagickReadImageBlob(wand, pinnedHeicData.data, (size_t) pinnedHeicData.length);
        
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
        
        unsigned char *resultBlobData = MagickGetImageBlob(wand, &resultBlobSize);
        
        jbyteArray resultData = env->NewByteArray((jsize) resultBlobSize);
        PinnedByteArray pinnedResultData(env, resultData);
        std::memcpy(pinnedResultData.data, resultBlobData, resultBlobSize);
        
        MagickRelinquishMemory(resultBlobData);
        
        return resultData;
    } catch (std::exception &error) {
        std::cerr << "ImageMagick.convertToJpegWithMaximumSize: " << error.what() << std::endl;
        return nullptr;
    }
}
