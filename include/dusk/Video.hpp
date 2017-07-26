#ifndef DUSK_VIDEO_HPP
#define DUSK_VIDEO_HPP

#include <dusk/Config.hpp>

namespace dusk
{

class Video
{
public:

    Video(const std::string& filename);
    virtual ~Video();

private:

    std::string _filename;

}; // class Video

} // namespace dusk

#endif // DUSK_VIDEO_HPP
