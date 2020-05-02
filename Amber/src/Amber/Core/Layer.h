#pragma once

#include <string>

#include "Amber/Core/Core.h"
#include "Amber/Core/Timestep.h"

#include "Amber/Events/Event.h"

namespace Amber 
{

class Layer 
{
public:
	Layer(const std::string& name = "Layer")
		: m_DebugName(name) {}
	virtual ~Layer() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(Event& event) {}

	inline const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;
};

}
