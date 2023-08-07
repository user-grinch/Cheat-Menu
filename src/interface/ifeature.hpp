#pragma once

template<typename T>
class IFeature {
  public:
    static T &Get() {
        static T _instance;
        return _instance;
    }
};