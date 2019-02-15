#include <iostream>

#include <Magick++/Image.h>
#include <cstring>

#include "at_yeoman_photobackup_server_heicToJpeg_HeicToJpeg.h"

using namespace Magick;

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

// TODO handle errors (exceptions?) from Magick++
JNIEXPORT jbyteArray JNICALL
Java_at_yeoman_photobackup_server_heicToJpeg_HeicToJpeg_convert(JNIEnv *env, jclass, jbyteArray heicData) {
    try {
        Image heicImage;
        PinnedByteArray pinnedHeicData(env, heicData);
        Blob heicDataBlob(pinnedHeicData.data, (size_t) pinnedHeicData.length);
        heicImage.read(heicDataBlob);
        Blob resultBlob;
        const std::string format = "JPEG";
        std::cerr << "write..." << std::endl;
        heicImage.write(&resultBlob);//, format, 16);
        std::cerr << "written." << std::endl;
        // TODO handle result length above Integer>MAX_VALUE
        jbyteArray resultData = env->NewByteArray((jsize) resultBlob.length());
        PinnedByteArray pinnedResultData(env, resultData);
        std::memcpy(pinnedResultData.data, resultBlob.data(), resultBlob.length());
        return resultData;
    } catch (std::exception &error) {
        std::cerr << "HeicToJpeg.convert: " << error.what() << std::endl;
        return nullptr;
    }
}
