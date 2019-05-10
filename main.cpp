#include <iostream>
#include <fstream>
#include <vector>

//#include <Magick++.h>

#include <MagickWand/MagickWand.h>

//using namespace Magick;

//int main() {
//    try {
//        InitializeMagick(nullptr);
//        Image heicImage;
//        std::ifstream file("e:/tmp/original.heic", std::ios::binary);
//        std::vector<char> heicData(
//                (std::istreambuf_iterator<char>(file)),
//                std::istreambuf_iterator<char>());
//        Blob heicDataBlob(heicData.data(), heicData.size());
//        std::cerr << "read..." << std::endl;
//        heicImage.magick("HEIC");
//        heicImage.read(heicDataBlob);
////        heicImage.read("e:/tmp/copy.jpg");
//        std::cerr << "read. original size: " << heicDataBlob.length() << " from " << heicData.size() << std::endl;
//        Blob resultBlob;
//        std::cerr << "write..." << std::endl;
//        heicImage.magick("PNG");
//        heicImage.write(&resultBlob, "JPEG");
////        heicImage.write("out.png");
//        std::cerr << "written. result size: " << resultBlob.length() << std::endl;
//    } catch (std::exception &error) {
//        std::cerr << error.what() << std::endl;
//    }
//    return 0;
//}


//int main(int argc, char **argv) {
//    try {
//        InitializeMagick(*argv);
//        std::cerr << "image" << std::endl;
//        Image in;
//        std::cerr << "magick" << std::endl;
//        in.magick("JPEG");
//        std::cerr << "read" << std::endl;
//        in.read("flower.jpg");
//        std::cerr << "done" << std::endl;
//    } catch (std::exception &error) {
//        std::cerr << error.what() << std::endl;
//        return 1;
//    }
//}

int main() {
    MagickWandGenesis();

    MagickWand *wand = nullptr;

    wand = NewMagickWand();
    // Read the image - all you need to do is change "logo:" to some other
    // filename to have this resize and, if necessary, convert a different file
    MagickReadImage(wand,"e:/tmp/original.heic");

    size_t size = 0;
    unsigned char * data = MagickGetImageBlob(wand, &size);

    /* Write the new image */
    MagickWriteImage(wand,"out.jpg");

    /* Clean up */
    if(wand)wand = DestroyMagickWand(wand);

    MagickWandTerminus();
}
