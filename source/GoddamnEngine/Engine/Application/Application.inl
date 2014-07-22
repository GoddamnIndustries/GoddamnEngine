
GD_NAMESPACE_BEGIN

	GDINL void Application::OnInitialize()
	{
		self->State = ApplicationState::Starting;
	}

	GDINL void Application::OnDestroy()
	{
		self->State = ApplicationState::Exiting;
	}

	GDINL void Application::OnSuspend()
	{
		self->State = ApplicationState::Suspended;
	}

	GDINL void Application::OnResumed()
	{
		self->State = ApplicationState::Running;
	}

GD_NAMESPACE_END
