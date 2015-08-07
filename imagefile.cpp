#include "imagefile.h"
#include "video.h"

ImageFile::ImageFile(Video *video)
    : MediaFile(MediaFile::ImageFileType),
      _video(video)
{

}

ImageFile::~ImageFile()
{

}

Video * ImageFile::video() const
{
    return _video;
}
