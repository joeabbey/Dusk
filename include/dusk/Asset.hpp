#ifndef DUSK_ASSET_HPP
#define DUSK_ASSET_HPP

#include <dusk/Config.hpp>

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <thread>
#include <type_traits>

namespace dusk
{

typedef unsigned long AssetId;

template <typename T>
class AssetCache
{
public:

    void Add(AssetId id, std::shared_ptr<T> asset)
    {
        _assets.emplace(id, asset);
    }

    std::shared_ptr<T> Get(AssetId id)
    {
        auto it = _assets.find(id);
        if (it == _assets.end())
        {
            return nullptr;
        }
        return it->second;
    }

    // Free all assets only owned by the cache
    void Purge()
    {
        for (auto it = _assets.begin(); it != _assets.end(); )
        {
            if (it->second.use_count() == 1)
            {
                _assets.erase(it++);
                continue;
            }

            ++it;
        }
    }

private:

    std::unordered_map<AssetId, std::shared_ptr<T>> _assets;

}; // class AssetCache<T>

template <typename T>
class AssetIndex
{
public:

    AssetId GetId(std::string str)
    {
        auto it = _index.find(str);
        if (it == _index.end())
        {
            AssetId id = _nextId++;
            _index.emplace(str, id);
            return id;
        }

        return it->second;
    }

private:

    AssetId _nextId = 1;

    std::unordered_map<std::string, AssetId> _index;

}; // class AssetIndex<T>

class ILoadable
{
public:

    virtual void Load() = 0;

    virtual inline bool IsLoaded() const { return _isLoaded; }

private:

    bool _isLoaded = false;

protected:

    virtual void SetLoaded(bool loaded) { _isLoaded = loaded; }

}; // class ILoadable

template <typename T>
class AssetLoader
{
    static_assert(std::is_base_of<T, ILoadable>::value, "T must derive from ILoadable");

public:

    AssetLoader()
    {
        _worker = std::thread(
            [this](){
                while (true)
                {
                    std::weak_ptr<T> tmp;

                    _queueMutex.lock();

                    if (_loadQueue.empty()) continue;
                    tmp = _loadQueue.front();
                    _loadQueue.pop();

                    _queueMutex.unlock();

                    if (std::shared_ptr<T> ptr = tmp.lock())
                    {
                        ptr->Load();
                    }
                }
            });
    }

private:

    std::thread _worker;

    std::mutex _queueMutex;
    std::queue<std::weak_ptr<T>> _loadQueue;

}; // class AssetLoader<T>

} // namespace dusk

#endif // DUSK_ASSET_HPP
