#ifndef DUSK_SOUND_HPP
#define DUSK_SOUND_HPP

#include <dusk/Config.hpp>

#include <string>
#include <vector>

namespace dusk
{

class Sound
{
public:

    Sound(const std::string& filename);
    virtual ~Sound();

    ALuint GetALBuffer() const { return _alBuffer; }

protected:

    ALuint  _alBuffer;
    ALenum  _alFormat;
    ALsizei _alFreq;

}; // class Sound

class SoundPlayer
{
public:

    SoundPlayer();
    virtual ~SoundPlayer();

    void Play(Sound * sound);
    ALint GetState();

protected:

    ALuint _alSource;

}; // class SoundPlayer

} // namespace dusk

#endif // DUSK_SOUND_HPP
