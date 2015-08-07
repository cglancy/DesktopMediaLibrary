#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include "mediafile.h"

class Video;

class ImageFile : public MediaFile
{
public:
    ImageFile(Video *video);
    ~ImageFile();

    Video * video() const;

private:
    Video *_video;
};

#endif // IMAGEFILE_H
