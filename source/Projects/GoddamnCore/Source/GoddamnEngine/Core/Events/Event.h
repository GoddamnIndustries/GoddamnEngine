#ifndef GD_CORE_EVENT_EVENT
#define GD_CORE_EVENT_EVENT

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>

#include <GoddamnEngine/Core/Events/EventFunctor.h>

GD_NAMESPACE_BEGIN

	template<class EventListener, class EventParameterType = handle>
	class EventQueue final
	{
		static_assert((sizeof(EventParameterType) <= sizeof(handle)), 
			"'Event<EventListener, ParameterType>' error: size of type 'ParameterType' should be "
			"less than pointer to make it simply copyable, what increases performance");

		static_assert((TypeTraits::IsPodType<EventParameterType>::Value),
			"'Event<EventListener, ParameterType>' error: type 'ParameterType' should be scalar.");

		typedef void (EventListener::* EventListenerFunction)(EventParameterType parameter);

	private:
		GD_CLASS_UNASSIGNABLE(EventQueue);
		GD_CLASS_UNCOPIABLE(EventQueue);

		Vector<EventListener*> Listeners;
		bool WasEventTriggered;

	public:
		EventParameterType const DefaultCallerParameter;	///< Caller parameter would be restored to this value
		EventParameterType CallerParameter;					///< Parameter that would be passed to listener function  

		EventListenerFunction const ListenerFunction;		///< Member-function would invoked for each listener on event launching

		/// @brief							Event constructor
		/// @param listenerFunction			Functor being invoke when event is launched
		/// @param defaultCallerParameter	Default parameter would be passed on listener 
		///									function invoked
		GDINL EventQueue(
			EventListenerFunction const listenerFunction, 
			EventParameterType const defaultCallerParameter = EventParameterType()
		) :
			ListenerFunction(listenerFunction),
			DefaultCallerParameter(defaultCallerParameter),
			CallerParameter(defaultCallerParameter),
			WasEventTriggered(false)
		{
		}

		/// @brief							Triggers event to make it launch-able via 
		///									'Trigger::LaunchEvent'.
		/// @brief callerParameter			Parameter would be passed to functions invokers
		GDINL void TriggerEvent(const EventParameterType callerParameter = EventParameterType())
		{
			this->CallerParameter = callerParameter;
			this->WasEventTriggered = true;
		}

		/// @brief					Launches event: all listener`s function would
		///							be invoked. This function works only if event 
		///							was triggered. Does nothing over wise.
		GDINL void LaunchEvent()
		{
			if(!this->WasEventTriggered)
			{
				return;
			}

			for(size_t cnt = 0; cnt < this->Listeners.GetLength(); cnt += 1)
			{
				EventListener* const eventListener = this->Listeners[cnt];
				(eventListener->*this->ListenerFunction)(this->CallerParameter);
			}

			this->CallerParameter = this->DefaultCallerParameter;
			this->WasEventTriggered = false;
		}

		/// @brief					Triggers event and launches it
		GDINL void TriggerAndLaunchEvent()
		{
			this->TriggerEvent();
			this->LaunchEvent();
		}

		/// @brief					Adds object as listener on event
		/// @param eventListener	Object, that would respond on event
		GDINL void operator+= (EventListener* const eventListener)
		{
			// GD_ASSERT(eventListener->GetTypeInformation().IsChildOf(EventFunctor::GetClassTypeInformation()),
			//	"'Event::operator+=' error: type of parameter 'eventListener' not derived from "
			//	"'EventListener' class");

			// GD_ASSERT(this->listeners.GetElementIndex(eventListener) == -1,
			//	"'Event::operator+=' error: value of parameter 'eventListener' already "
			//	"exists in this event listeners list");

			this->Listeners.PushLast(eventListener);
			this->WasEventTriggered = false;
		}

		/// @brief					Removes object from event listeners list
		/// @param eventListener	Object that is going to be removed from listeners list
		GDINL void operator-= (EventListener* const eventListener)
		{
			const size_t eventListenerIndex = this->Listeners.FindFirstElement(eventListener);
			GD_ASSERT(eventListenerIndex != -1,
				"'Event::operator+=' error: value of parameter 'eventListener' does "
				"not exists in this event listners list");

			this->Listeners.RemoveElementAt(eventListenerIndex);
			this->WasEventTriggered = false;
		}
	};

GD_NAMESPACE_END

#endif