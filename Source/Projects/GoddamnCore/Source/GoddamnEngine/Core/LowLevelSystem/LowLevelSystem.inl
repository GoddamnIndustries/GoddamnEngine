
GD_NAMESPACE_BEGIN

	GDINL LowLevelEventListenerProc LowLevelSystem::GetEventCallBack(LowLevelEvent const event) const
	{
		return this->eventListeners[((size_t)event)];
	}

	GDINL void LowLevelSystem::SetEventCallBack(LowLevelEvent const event,
												LowLevelEventListenerProc const callback)
	{
		GD_ASSERT((event < GD_LL_EVENT_UNKNOWN),
				  "Unknown low level event specified");
		
		this->eventListeners[((size_t)event)] = ((callback != nullptr)
			? callback : (&LowLevelSystem::WindowWrapperEventListenerDummpyProc));
	}
	
GD_NAMESPACE_END
