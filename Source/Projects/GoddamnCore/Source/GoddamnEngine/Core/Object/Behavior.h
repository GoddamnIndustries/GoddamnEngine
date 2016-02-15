		/*!
		 * @brief Callback being triggered after this object is initialized (or reset).
		 * This function may be useful for assigning properties default values. 
		 */
		GDAPI virtual void OnInit() {}

		/*!
		 * @brief Callback being triggered each frame.
		 * This function may be useful for implementing gameplay-related logic. 
		 */
		GDAPI virtual void OnUpdate() {}
		
		/*!
		 * @brief Callback being triggered each frame from the renderer thread.
		 * This function may be useful for implementing rendering-related logic. 
		 */
		GDAPI virtual void OnRender() {}