#pragma once

#include "../EngineBase.h"
#undef CreateFile
#undef MoveFile
#undef SetCurrentDirectory
#undef GetCurrentDirectory

namespace Engine
{
	namespace IO
	{
		class FileAccessException : public Exception
		{
		public:
			string ToString() const override;
		};
		class FileReadEndOfFileException : public Exception
		{
		public:
			UInt32 DataRead;
			FileReadEndOfFileException(UInt32 data_read);
			string ToString() const override;
		};
		class FileFormatException : public Exception
		{
		public:
			string ToString() const override;
		};

		enum FileAccess
		{
			AccessRead,
			AccessWrite,
			AccessReadWrite
		};
		enum FileCreationMode
		{
			CreateAlways,
			CreateNew,
			OpenAlways,
			OpenExisting,
			TruncateExisting
		};
		enum SeekOrigin
		{
			Begin,
			Current,
			End
		};

#ifdef ENGINE_WINDOWS
		constexpr widechar PathChar = L'\\';
		constexpr widechar* NewLineChar = L"\r\n";
		constexpr Encoding TextFileEncoding = Encoding::ANSI;
#endif
#ifdef ENGINE_UNIX
		constexpr widechar PathChar = L'/';
		constexpr widechar* NewLineChar = L"\n";
		constexpr Encoding TextFileEncoding = Encoding::UTF8;
#endif

		string NormalizePath(const string& path);
		string ExpandPath(const string& path);
		handle CreateFile(const string& path, FileAccess access, FileCreationMode mode);
		handle CreateFileTemporary(const string& path, FileAccess access, FileCreationMode mode, bool delete_on_close);
		void CreatePipe(handle* pipe_in, handle* pipe_out);
		handle GetStandartOutput();
		handle GetStandartInput();
		handle GetStandartError();
		void SetStandartOutput(handle file);
		void SetStandartInput(handle file);
		void SetStandartError(handle file);
		void CloseFile(handle file);
		void Flush(handle file);
		UInt64 GetFileSize(handle file);
		void MoveFile(const string& from, const string& to);
		bool FileExists(const string& path);
		void ReadFile(handle file, void* to, UInt32 amount);
		void WriteFile(handle file, const void* data, UInt32 amount);
		Int64 Seek(handle file, Int64 position, SeekOrigin origin);
		void SetFileSize(handle file, UInt64 size);
		void RemoveFile(const string& path);
		void SetCurrentDirectory(const string& path);
		string GetCurrentDirectory();
	}
}
