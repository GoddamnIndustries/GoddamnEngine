// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Allocator implementation.
 */
//! @todo Fix include paths.
//#include <GoddamnEngine/Core/Platform/PlatformAssert.h>
#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_GNU_LINUX

#pragma push_macro("implicit")
#undef implicit
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#pragma pop_macro("implicit")

GD_NAMESPACE_BEGIN

	enum class GtkPlusDialogCommand
	{
		CommandDialogClosed,
		CommandButtonClicked,
	};  // enum class GtkPlusDialog

	using GtkPlusDialogID = IntPtr;
	using GtkPlusDialogProc = void(*)(class GtkPlusDialog* const dialog, GtkPlusDialogCommand const dialogCommand, GtkPlusDialogID const dialogCommandID);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! GTK+ dialog.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL GtkPlusDialog final : public TNonCopyable
	{
	private:
		GtkWidget* m_Dialog;
		GtkWidget* m_DialogTopContainer;
		GtkPlusDialogProc m_DialogProc;
		GtkPlusDialogID m_DialogCloseCode;
	public:
		GDINL explicit GtkPlusDialog(GtkPlusDialogProc const dialogProc, gchar const* const dialogTitle
			, gint const dialogWidth, gint const dialogHeight, gboolean dialogResizable = FALSE, guint const dialogBorderWidth = 3)
			: m_Dialog(gtk_window_new(GTK_WINDOW_TOPLEVEL)), m_DialogTopContainer(m_Dialog), m_DialogProc(dialogProc), m_DialogCloseCode(0)
		{
			gtk_window_set_title(GTK_WINDOW(m_Dialog), GD_ASSERT_DIALOG_TITLE);
			gtk_window_set_position(GTK_WINDOW(m_Dialog), GTK_WIN_POS_CENTER_ALWAYS);
			gtk_window_set_default_size(GTK_WINDOW(m_Dialog), dialogWidth, dialogHeight);
			gtk_window_set_resizable(GTK_WINDOW(m_Dialog), dialogResizable);
			gtk_container_set_border_width(GTK_CONTAINER(m_DialogTopContainer), dialogBorderWidth);
			g_signal_connect(G_OBJECT(m_Dialog), "destroy", G_CALLBACK(&CallBackDialogClosed), NULL);
		}
	public:
		/*!
		 * Starts the dialog.
		 */
		GDINL void StartDialog()
		{
			gtk_widget_show_all(m_Dialog);
			gtk_main();
		}
		/*!
		 * Closes the dialog with code.
		 */
		GDINL void CloseDialog(GtkPlusDialogID const dialogEndCode)
		{
			gtk_main_quit();
			gtk_window_close(GTK_WINDOW(m_Dialog));
			m_DialogCloseCode = dialogEndCode;
		}
		/*!
		 * Returns code that was passed to @c CloseDialog function.
		 */
		GDINL GtkPlusDialogID GetCloseCode() const
		{
			return m_DialogCloseCode;
		}
	private:
		GDINL GtkWidget* ContainerAdd(GtkWidget* const widget)
		{
			gtk_container_add(GTK_CONTAINER(m_DialogTopContainer), widget);
			return widget;
		}
		GDINL GtkWidget* ContainerBegin(GtkWidget* const container)
		{
			ContainerAdd(container);
			g_object_set_data(G_OBJECT(container), "m_ParentContainer", m_DialogTopContainer);
			return m_DialogTopContainer = container;
		}
	public:
		/*!
		 * Adds a new label to the current container.
		 */
		GDINL GtkWidget* AddLabel(gchar const* const labelText)
		{
			auto const label = gtk_label_new(labelText);
			gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
			gtk_label_set_line_wrap(GTK_LABEL(label), GD_TRUE);
			return ContainerAdd(label);
		}
		/*!
		 * Adds a new button to the current container.
		 */
		GDINL GtkWidget* AddButton(gchar const* const buttonText, GtkPlusDialogID const buttonID = 0, gboolean const buttonActive = GD_TRUE)
		{
			auto const button = gtk_button_new_with_label(buttonText);
			gtk_widget_set_sensitive(button, buttonActive);
			if (buttonActive == GD_TRUE)
			{
				g_object_set_data(G_OBJECT(button), "m_ButtonID", reinterpret_cast<gpointer>(buttonID));
				g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(&CallBackButtonClicked), this);
			}
			return ContainerAdd(button);
		}
		/*!
		 * Adds a new non-editable text-view to the current container.
		 */
		GDINT GtkWidget* AddTextView(gchar const* const text)
		{
			auto const textView = gtk_text_view_new();
			auto const textViewBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
			gtk_text_buffer_set_text(textViewBuffer, text, -1);
			gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);
			gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_WORD);
			return ContainerAdd(textView);
		}
	public:
		/*!
		 * Pushes new box container.
		 */
		GDINL GtkWidget* BeginBox(GtkOrientation const boxOrientation, guint const boxSpacing = 3)
		{
			auto const box = gtk_box_new(boxOrientation, boxSpacing);
			gtk_container_set_border_width(GTK_CONTAINER(box), 3);
			return ContainerBegin(box);
		}
		/*!
		 * Pushes new frame container.
		 */
		GDINL GtkWidget* BeginFrame(gchar const* const frameText, guint const frameSpacing = 3)
		{
			auto const frame = gtk_frame_new(frameText);
			gtk_container_set_border_width(GTK_CONTAINER(frame), frameSpacing);
			return ContainerBegin(frame);
		}
		/*!
		 * Pops top container.
		 */
		GDINL GtkWidget* End()
		{
			return m_DialogTopContainer = static_cast<GtkWidget*>(g_object_get_data(G_OBJECT(m_DialogTopContainer), "m_ParentContainer"));
		}
	private:
		GDINT static void CallBackDialogClosed(GtkWindow*, GtkPlusDialog* const dialog)
		{
			if (dialog != nullptr && dialog->m_DialogProc != nullptr)
			{
				dialog->m_DialogProc(dialog, GtkPlusDialogCommand::CommandDialogClosed, 0);
			}
		}
		GDINT static void CallBackButtonClicked(GtkButton* const dialogButton, GtkPlusDialog* const dialog)
		{
			if (dialog->m_DialogProc != nullptr)
			{
				auto const dialogButtonID = reinterpret_cast<GtkPlusDialogID>(g_object_get_data(G_OBJECT(dialogButton), "m_ButtonID"));
				dialog->m_DialogProc(dialog, GtkPlusDialogCommand::CommandButtonClicked, dialogButtonID);
			}
		}
	};  // class GtkPlusDialog

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Runtime checks.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL LinuxPlatformAssert final : public IPlatformAssert
	{
	private:
		GDINT virtual AssertDialogResult RunAssertDialog(AssertData const* const data) override final;
		GDINT virtual void ReportAndExit(AssertData const* const assertData) override final;
	private:
		GDINT static void AssertDialogProc(GtkPlusDialog* const dialog, GtkPlusDialogCommand const dialogCommand, GtkPlusDialogID const dialogCommandID);
	};	// class LinuxPlatformAssert

	GD_IMPLEMENT_SINGLETON(IPlatformAssert, LinuxPlatformAssert)

	// ------------------------------------------------------------------------------------------
	// Assert utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Runs an 'assert failed' dialog.
	 *
	 * @param assertData Assertion information.
	 * @returns Dialog result.
	 */
	GDINT AssertDialogResult LinuxPlatformAssert::RunAssertDialog(AssertData const* const assertData)
	{
		gtk_init(nullptr, nullptr);
		gdk_beep();

		GtkPlusDialog dialog(&AssertDialogProc, GD_ASSERT_DIALOG_TITLE, 512, 256);
		dialog.BeginBox(GTK_ORIENTATION_HORIZONTAL);
		{
			dialog.BeginFrame(GD_ASSERT_DIALOG_GROUP_BOX_TEXT);
			{
				dialog.BeginBox(GTK_ORIENTATION_VERTICAL);
				{
					static gchar assertMessageDesc[16 * 1024] = {};
					if (assertData->AssertExpression != nullptr)
					{
						g_sprintf(assertMessageDesc, "%s\n\nAt: \n[File]: %s\n[Line]: %u\n[Function]: %s\n[Expression]: %s\n"
							, assertData->AssertMessage, assertData->AssertFileName, assertData->AssertLineNumber, assertData->AssertFunctionName, assertData->AssertExpression);
					}
					else
					{
						g_sprintf(assertMessageDesc, "%s\n\nAt: \n[File]: %s\n[Line]: %u\n[Function]: %s\n"
							, assertData->AssertMessage, assertData->AssertFileName, assertData->AssertLineNumber, assertData->AssertFunctionName);
					}

					dialog.AddLabel(GD_ASSERT_DIALOG_DESCRIPTION_TEXT);
					dialog.AddTextView(assertMessageDesc);
				}
				dialog.End();
			}
			dialog.End();
			dialog.BeginBox(GTK_ORIENTATION_VERTICAL);
			{
				dialog.AddButton(GD_ASSERT_DIALOG_BTN_ABORT_TEXT,      ASSERT_DIALOG_BTN_ABORT);
				dialog.AddButton(GD_ASSERT_DIALOG_BTN_RETRY_TEXT,      ASSERT_DIALOG_BTN_RETRY,      assertData->AssertExpression != nullptr);
				dialog.AddButton(GD_ASSERT_DIALOG_BTN_IGNORE_TEXT,     ASSERT_DIALOG_BTN_IGNORE,     assertData->AssertExpression != nullptr);
				dialog.AddButton(GD_ASSERT_DIALOG_BTN_IGNORE_ALL_TEXT, ASSERT_DIALOG_BTN_IGNORE_ALL, assertData->AssertExpression != nullptr);
				dialog.AddButton(GD_ASSERT_DIALOG_BTN_REPORT_TEXT,     ASSERT_DIALOG_BTN_REPORT);
				dialog.AddButton(GD_ASSERT_DIALOG_BTN_BREAK_TEXT,      ASSERT_DIALOG_BTN_BREAK,      GD_DEBUG);
			}
			dialog.End();
		}
		dialog.End();

		dialog.StartDialog();
		return static_cast<AssertDialogResult>(dialog.GetCloseCode());
	}

	/*!
	 * Reports that the assertion has failed and quits the application.
	 * @param assertData Assertion information.
 	 */
	GDINT void LinuxPlatformAssert::ReportAndExit(AssertData const* const assertData)
	{
		GD_NOT_USED_L(assertData);
		exit(-1);
	}

	/*!
	 * Handles messages for dialog window by GTK+.
	 */
	GDINT void LinuxPlatformAssert::AssertDialogProc(GtkPlusDialog* const dialog, GtkPlusDialogCommand const dialogCommand, GtkPlusDialogID const dialogCommandID)
	{
		switch (dialogCommand)
		{
			case GtkPlusDialogCommand::CommandDialogClosed:
				dialog->CloseDialog(ASSERT_DIALOG_BTN_ABORT);
				break;
			case GtkPlusDialogCommand::CommandButtonClicked:
				dialog->CloseDialog(dialogCommandID);
				break;
		}
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_GNU_LINUX
