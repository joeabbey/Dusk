#include "dusk/Sound.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

namespace dusk
{

Sound::Sound(const std::string& filename)
{
    FILE * file;
    OggVorbis_File vf;
    vorbis_info * vInfo;
    char buffer[32768];     // 32 KB buffer
    int endian = 0;         // 0 = Little-Endian, 1 = Big-Endian
    int bitStream;
    long bytes;
    std::vector<unsigned char> data;

    alGenBuffers(1, &_alBuffer);

    DuskLogInfo("Loading sound file '%s'", filename.c_str());

    file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        DuskLogError("Failed to load sound file '%s'", filename.c_str());
        return;
    }

    if (ov_open_callbacks(file, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
    {
        DuskLogError("Failed to open ogg callbacks");
        return;
    }

    vInfo = ov_info(&vf, -1);
    if (1 == vInfo->channels)
    {
        _alFormat = AL_FORMAT_MONO16;
    }
    else
    {
        _alFormat = AL_FORMAT_STEREO16;
    }

    _alFreq = vInfo->rate;

    do
    {
        bytes = ov_read(&vf, buffer, sizeof(buffer), endian, 2, 1, &bitStream);
        data.insert(data.end(), buffer, buffer + bytes);
    }
    while (bytes > 0);

    alBufferData(_alBuffer, _alFormat, data.data(), static_cast<ALsizei>(data.size()), _alFreq);

    ov_clear(&vf);
}

Sound::~Sound()
{
    alDeleteBuffers(1, &_alBuffer);
}

SoundPlayer::SoundPlayer()
{
    alGenSources(1, &_alSource);

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(_alSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
}

SoundPlayer::~SoundPlayer()
{
    alDeleteSources(1, &_alSource);
}

void SoundPlayer::Play(Sound * sound)
{
    alSourcei(_alSource, AL_BUFFER, sound->GetALBuffer());

    alSourcePlay(_alSource);
}

ALint SoundPlayer::GetState()
{
    ALint state;
    alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
    return state;
}

} // namespace dusk
