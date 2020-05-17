#ifndef ASSET_H_
#define ASSET_H_

class Asset
{
private:
   // variables
   int data;
   int width;
   int height;
   float aspect;

public:
   void Load(const char* filename);
   void Render(unsigned width, unsigned height);

   // accessors to be used when you need to get the asset data directly from outseide the class, for instance when
   // blending two images or combining them in any way
   int get_width() { return width; }
   int get_height() { return height; }
   float get_aspect() { return aspect; }
   int get_texture() { return data; }
};

#endif