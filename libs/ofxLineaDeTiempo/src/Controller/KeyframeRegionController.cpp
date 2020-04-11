//
//  KeyframeRegionController.cpp
//  tracksAndTimeTest
//
//  Created by Roy Macdonald on 3/17/20.
//

#include "LineaDeTiempo/Controller/KeyframeRegionController.h"
#include "LineaDeTiempo/Controller/KeyframeTrackController.h"
#include "LineaDeTiempo/Controller/KeyframeCollectionController.h"
#include "LineaDeTiempo/View/KeyframesRegionView.h"
#include "LineaDeTiempo/Utils/ConstVars.h"



namespace ofx {
namespace LineaDeTiempo {


template<typename T>
KeyframeRegionController_<T>::KeyframeRegionController_(const std::string& name, const ofRange64u& timeRange, TrackController* parentTrack, TimeControl* timeControl)
: RegionController(name, timeRange, parentTrack, timeControl)
, _parentTrack(dynamic_cast<KeyframeTrackController_<T>*>(parentTrack))
{
	_dataTypeName = typeid(T).name();
		
	for(size_t i = 0; i < type_dimensions<T>::value; i++)
	{
		_collections.push_back(addChild<KeyframeCollectionController<T>>(this, i ));
	}
}

template<typename T>
size_t  KeyframeRegionController_<T>::getNumDimensions() const
{
	return  type_dimensions<T>::value;
}

template<typename T>
void KeyframeRegionController_<T>::generateView()
{
	if(getView() == nullptr){
		auto p = _parentTrack;
		if(p && p->getView())
		{
			_keyframesRegionView = p->getView()->template addRegion<KeyframesRegionView, KeyframeRegionController_<T>>(this);
			
			setView(_keyframesRegionView);
		}
		
		generateChildrenViews(this);
	}
}

template<typename T>
void KeyframeRegionController_<T>::destroyView()
{
	if(getView()){
		destroyChildrenViews(this);
		
		
		if(_parentTrack && _parentTrack->getView())
		{
			
			if(_parentTrack->getView()->removeRegion(this) == false)
			{
				ofLogError("KeyframeRegionController_<T>::destroyView") << "Could not remove region correctly. " << getId();
			}
			setView(nullptr);
			_keyframesRegionView = nullptr;
		}
	}
}

template<typename T>
void KeyframeRegionController_<T>::removeAllKeyframes()
{
	for(auto c : _collections)
	{
		c->removeAllChildren();
	}
}

template<typename T>
bool KeyframeRegionController_<T>:: update(uint64_t& t)
{
	return _update(t, _parentTrack->getParameter());
}

template<typename T>
bool KeyframeRegionController_<T>::_update(const uint64_t& t, ofParameter<T>& param)
{
	bool bUpdated = false;
	
	T p = param.get();
	
	for(auto c : _collections)
	{
		bUpdated |= c->update(t, p);
	}
	if(bUpdated)
	{
		param = p;
	}
	
	return bUpdated;
}
//
//template<>
//bool KeyframeRegionController_<void>::_update(const uint64_t& t, ofParameter<void>& param)
//{
//	bool bUpdated = false;
//
//	for(auto c : _collections)
//	{
//		bUpdated |= c->update(t, param);
//	}
//
//
//	return bUpdated;
//}



template<typename T>
KeyframesRegionView*  KeyframeRegionController_<T>::getKeyframesRegionView()
{
	return _keyframesRegionView;
}

template<typename T>
const KeyframesRegionView* KeyframeRegionController_<T>::getKeyframesRegionView() const
{
	return _keyframesRegionView;
}
template<typename T>
KeyframeTrackController_<T>* KeyframeRegionController_<T>::getParentKeyframeTrack()
{
	return _parentTrack;
}

template<typename T>
const KeyframeTrackController_<T>* KeyframeRegionController_<T>::getParentKeyframeTrack()const
{
	return _parentTrack;
}



template<typename T>
void KeyframeRegionController_<T>::fromJson(const ofJson& j)
{
	
	RegionController::fromJson(j);

	
	if(j.count("_collections")== 0 || j.count("_dataTypeName") == 0)
	{
		ofLogError("KeyframeRegionController_<T>::fromJson") << "failed. Malformed json. DataTypeName or keyframedData elements not present";
		return;
	}
	
	auto dt = j["_dataTypeName"].get<std::string>();
	
	if(dt != getDataTypeName())
	{
		ofLogError("KeyframeRegionController_<T>::fromJson") << "failed. DataType seems to be different to the one saved on file";
		return;
	}

	if(!j["_collections"].is_array())
	{
		ofLogError("KeyframeRegionController_<T>::fromJson") << "failed. _collections object is not an array";
		return;
	}
	
	auto& c = j["_collections"];
	
	for(size_t i = 0; i < c.size(); ++i )
	{
		_collections[i]->fromJson(c[i]);
	}

	

}

template<typename T>
ofJson KeyframeRegionController_<T>::toJson()
{
	ofJson j = RegionController::toJson();
	j["class"] = "KeyframeRegionController_";


	j["_collections"] = nlohmann::json::array();
	for(auto c : _collections)
	{
		if(c) j["_collections"].emplace_back(c->toJson()) ;
	}
	

	return j;
}




template class KeyframeRegionController_<glm::vec2>;
template class KeyframeRegionController_<glm::vec3>;
template class KeyframeRegionController_<glm::vec4>;

template class KeyframeRegionController_<bool>;
//template class KeyframeRegionController_<void>;

template class KeyframeRegionController_<         char>;
template class KeyframeRegionController_<unsigned char>;
template class KeyframeRegionController_<  signed char>;
template class KeyframeRegionController_<         short>;
template class KeyframeRegionController_<unsigned short>;
template class KeyframeRegionController_<         int>;
template class KeyframeRegionController_<unsigned int>;
template class KeyframeRegionController_<         long>;
template class KeyframeRegionController_<unsigned long>;
template class KeyframeRegionController_<         long long>;
template class KeyframeRegionController_<unsigned long long>;
template class KeyframeRegionController_<float>;
template class KeyframeRegionController_<double>;
template class KeyframeRegionController_<long double>;

template class KeyframeRegionController_<ofColor_<char>>;
template class KeyframeRegionController_<ofColor_<unsigned char>>;
template class KeyframeRegionController_<ofColor_<short>>;
template class KeyframeRegionController_<ofColor_<unsigned short>>;
template class KeyframeRegionController_<ofColor_<int>>;
template class KeyframeRegionController_<ofColor_<unsigned int>>;
template class KeyframeRegionController_<ofColor_<long>>;
template class KeyframeRegionController_<ofColor_<unsigned long>>;
template class KeyframeRegionController_<ofColor_<float>>;
//template class KeyframeRegionController_<ofColor_<double>>;


} } // ofx::LineaDeTiempo

