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

class AudioChannel
{
public:

    AudioChannel();
    virtual ~AudioChannel();

    static void SetListenerPosition(glm::vec3 pos);

    void Play(Sound * sound);
    ALint GetState();

    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition() const { return _position; }

protected:

    static glm::vec3 _ListenerPosition;

    glm::vec3 _position;

    ALuint _alSource;

}; // class AudioChannel

} // namespace dusk

#endif // DUSK_SOUND_HPP
