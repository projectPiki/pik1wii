#ifndef EGG_CORE_SCENE_MANAGER_H
#define EGG_CORE_SCENE_MANAGER_H

namespace EGG {

class SceneManager {
public:
	virtual void calc();               // _08
	virtual void draw();               // _0C
	virtual void calcCurrentScene();   // _10
	virtual void calcCurrentFader();   // _14
	virtual void drawCurrentScene();   // _18
	virtual void drawCurrentFader();   // _1C
	virtual void createDefaultFader(); // _20
};

} // namespace EGG

#endif
