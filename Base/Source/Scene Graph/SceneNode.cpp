#include "SceneNode.h"


SceneNode::SceneNode()
: mesh(nullptr)
, transform(glm::mat4(100.f))
, Node()
{
}


SceneNode::~SceneNode()
{
}
