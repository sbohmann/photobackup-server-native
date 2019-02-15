#include <iostream>

#include <MagickWand/MagickWand.h>
#include <cstring>

#include "at_yeoman_photobackup_server_heicToJpeg_HeicToJpeg.h"

JNIEXPORT void JNICALL
Java_at_yeoman_photobackup_server_heicToJpeg_HeicToJpeg_initialize(JNIEnv *, jclass) {
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

// TODO handle errors from Magick++
JNIEXPORT jbyteArray JNICALL
Java_at_yeoman_photobackup_server_heicToJpeg_HeicToJpeg_convert(JNIEnv *env, jclass, jbyteArray heicData) {
    try {
        Image heicImage;
        PinnedByteArray pinnedHeicData(env, heicData);

        MagickWand *wand = nullptr;

        wand = NewMagickWand();

        MagickReadImageBlob(wand, pinnedHeicData.data, (size_t) pinnedHeicData.length);

        MagickSetImageFormat(wand, "JPEG");

        size_t resultBlobSize = 0;
        unsigned char * resultBlobData = MagickGetImageBlob(wand, &resultBlobSize);

        if(wand)wand = DestroyMagickWand(wand);

        jbyteArray resultData = env->NewByteArray((jsize) resultBlobSize);
        PinnedByteArray pinnedResultData(env, resultData);
        std::memcpy(pinnedResultData.data, resultBlobData, resultBlobSize);
        
        MagickRelinquishMemory(resultBlobData);

        return resultData;
    } catch (std::exception &error) {
        std::cerr << "HeicToJpeg.convert: " << error.what() << std::endl;
        return nullptr;
    }
}
