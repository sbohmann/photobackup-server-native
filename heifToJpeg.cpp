#include <iostream>

#include <Magick++/Image.h>
#include <cstring>

#include "at_yeoman_photobackup_server_heicToJpeg_HeicToJpeg.h"

using namespace Magick;

struct ByteArray {
    JNIEnv *const env;
    jbyteArray const array;
    jbyte *const data;
    jsize const length;

    ByteArray(JNIEnv *env, jbyteArray array)
            : env(env),
              array(array),
              data(env->GetByteArrayElements(array, 0)),
              length(env->GetArrayLength(array)) {
    }

    ~ByteArray() {
        env->ReleaseByteArrayElements(array, data, 0);
    }
};

// TODO handle errors (exceptions?) from Magick++
JNIEXPORT jbyteArray JNICALL
Java_at_yeoman_photobackup_server_heicToJpeg_HeicToJpeg_convert(JNIEnv *env, jclass, jbyteArray heicData) {
    Image heicImage;
    ByteArray pinnedHeicData(env, heicData);
    Blob heicDataBlob(pinnedHeicData.data, (size_t) pinnedHeicData.length);
    heicImage.read(heicDataBlob);
    Blob resultBlob;
    heicImage.write(&resultBlob, "HEIC");
    // TODO handle result length above Integer>MAX_VALUE
    jbyteArray resultData = env->NewByteArray((jsize) resultBlob.length());
    ByteArray pinnedResultData(env, resultData);
    std::memcpy(pinnedResultData.data, resultBlob.data(), resultBlob.length());
    return resultData;
}
