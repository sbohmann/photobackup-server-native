#include <iostream>
#include <fstream>
#include <vector>

#include <Magick++/Image.h>

using namespace Magick;

int main() {
    try {
        Image heicImage;
        std::ifstream file("e:/tmp/copy.jpg", std::ios::binary);
        std::vector<char> heicData(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
        Blob heicDataBlob(heicData.data(), heicData.size());
        std::cerr << "read..." << std::endl;
        heicImage.read(heicDataBlob);
        std::cerr << "read." << std::endl;
        Blob resultBlob;
        std::cerr << "write..." << std::endl;
        heicImage.write(&resultBlob);
        std::cerr << "written." << std::endl;
    } catch (std::exception &error) {
        std::cerr << error.what() << std::endl;
    }
    return 0;
}
