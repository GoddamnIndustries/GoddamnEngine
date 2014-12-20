
GD_NAMESPACE_BEGIN

	GDINL void Application::OnInitialize()
	{
		this->State = ApplicationState::Starting;
	}

	GDINL void Application::OnDestroy()
	{
		this->State = ApplicationState::Exiting;
	}

	GDINL void Application::OnSuspend()
	{
		this->State = ApplicationState::Suspended;
	}

	GDINL void Application::OnResumed()
	{
		this->State = ApplicationState::Running;
	}

GD_NAMESPACE_END
