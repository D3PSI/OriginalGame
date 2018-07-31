#include "HUD.hpp"

std::vector<HUD> hudElements;

/*
*	Constructor.
*	
*/
HUD::HUD() {
	// TODO:	HUD element creation etc.
	hudElements.push_back(*this);
}

/*
*	Loops through all the elements in the hudElements vector and renders them.
*	
*/
void HUD::renderAll() {
	for (unsigned int i = 0; i < hudElements.size(); i++) {
		hudElements[i].render();
	}
}

/*
*	Renders a HUD element.
*	
*/
void HUD::render() {
	
}

/*
*	Destructor.
*	
*/
HUD::~HUD() {

}
