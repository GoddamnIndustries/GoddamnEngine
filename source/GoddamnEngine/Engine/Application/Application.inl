
GD_NAMESPACE_BEGIN

	GDINL ApplicationState Application::GetApplicationState() const	 
	{
		return self->state;
	}

	GDINL void Application::OnInitialize()
	{
		self->state = ApplicationState::Starting;
	}

	GDINL void Application::OnDestroy()
	{
		self->state = ApplicationState::Exiting;
	}

	GDINL void Application::OnSuspend()
	{
		self->state = ApplicationState::Suspended;
	}

	GDINL void Application::OnResumed()
	{
		self->state = ApplicationState::Running;
	}

GD_NAMESPACE_END
