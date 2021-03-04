
#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H
 
#include <string>
#define GLFW_DLL
#include <GLFW/glfw3.h>
constexpr int tileSize = 32;
constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;
struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};
 
 
struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return  data; }

  Pixel GetPixel(int x, int y) { return data[width * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix) { data[width* y + x] = pix; }
  
  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  bool self_allocated = false;
};
struct Anim
{
  Image *f1;
  Image *f2;
  Image *f3;
};


#endif //MAIN_IMAGE_H
