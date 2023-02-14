#pragma once

#include<memory>

class GameObject;

struct ProxyCameraOpDesc
{

};

class ProxyCamera
{

public:



    std::shared_ptr<GameObject> CameraObject;
};
