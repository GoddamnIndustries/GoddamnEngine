#include "FileApi.h"
#include "../Miscellaneous/DynamicString.h"

#include <Windows.h>

#undef CreateFile
#undef MoveFile
#undef SetCurrentDirectory
#undef GetCurrentDirectory

namespace Engine
{
	namespace IO
	{
		string FileAccessException::ToString() const
		{
			return L"FileAccessException";
		}
		FileReadEndOfFileException::FileReadEndOfFileException(UInt32 data_read)
		    : DataRead(data_read)
		{
		}
		string FileReadEndOfFileException::ToString() const
		{
			return L"FileReadEndOfFileException: Data read amount = ";
		} // +string(DataRead);

		string FileFormatException::ToString() const
		{
			return L"FileFormatException";
		}
		string NormalizePath(const string& path)
		{
			/*if (IO::PathChar == L'\\') return path.Replace(L'/', L'\\');
			else if (PathChar == L'/') return path.Replace(L'\\', L'/');
			return L"";*/
			return path;
		}
		string ExpandPath(const string& path)
		{
			string Path = NormalizePath(path);
			DynamicString exp;
			exp.Resize(0x1000);
			do
			{
				auto result = GetFullPathNameW(Path.CStr(), exp.GetCapacity(), exp.CStr(), 0);
				if (!result)
					throw Exception();
				if (result > UInt32(exp.GetLength()))
					exp.Reserve(result);
				else
					break;
			} while (true);
			return exp.CStr();
		}
		handle CreateFile(const string& path, FileAccess access, FileCreationMode mode)
		{
			DWORD Access = 0;
			DWORD Share = 0;
			if (access == FileAccess::AccessRead)
			{
				Access = GENERIC_READ;
				Share = FILE_SHARE_READ;
			}
			else if (access == FileAccess::AccessWrite)
			{
				Access = GENERIC_WRITE;
			}
			else if (access == FileAccess::AccessReadWrite)
			{
				Access = GENERIC_READ | GENERIC_WRITE;
			}
			DWORD Creation = 0;
			if (mode == FileCreationMode::CreateAlways)
				Creation = CREATE_ALWAYS;
			else if (mode == FileCreationMode::CreateNew)
				Creation = CREATE_NEW;
			else if (mode == FileCreationMode::OpenAlways)
				Creation = OPEN_ALWAYS;
			else if (mode == FileCreationMode::OpenExisting)
				Creation = OPEN_EXISTING;
			else if (mode == FileCreationMode::TruncateExisting)
				Creation = TRUNCATE_EXISTING;
			DWORD Flags = FILE_ATTRIBUTE_NORMAL;
			handle file = ::CreateFileW(NormalizePath(path).CStr(), Access, Share, 0, Creation, Flags, 0);
			if (file == INVALID_HANDLE_VALUE)
				throw FileAccessException();
			return file;
		}
		handle CreateFileTemporary(const string& path, FileAccess access, FileCreationMode mode, bool delete_on_close)
		{
			if (delete_on_close && mode != CreateNew)
				throw InvalidArgumentException();
			DWORD Access = 0;
			DWORD Share = 0;
			if (access == FileAccess::AccessRead)
			{
				Access = GENERIC_READ;
				Share = FILE_SHARE_READ;
			}
			else if (access == FileAccess::AccessWrite)
			{
				Access = GENERIC_WRITE;
			}
			else if (access == FileAccess::AccessReadWrite)
			{
				Access = GENERIC_READ | GENERIC_WRITE;
			}
			DWORD Creation = 0;
			if (mode == FileCreationMode::CreateAlways)
				Creation = CREATE_ALWAYS;
			else if (mode == FileCreationMode::CreateNew)
				Creation = CREATE_NEW;
			else if (mode == FileCreationMode::OpenAlways)
				Creation = OPEN_ALWAYS;
			else if (mode == FileCreationMode::OpenExisting)
				Creation = OPEN_EXISTING;
			else if (mode == FileCreationMode::TruncateExisting)
				Creation = TRUNCATE_EXISTING;
			DWORD Flags = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY;
			if (delete_on_close)
				Flags |= FILE_FLAG_DELETE_ON_CLOSE;
			handle file = ::CreateFileW(NormalizePath(path).CStr(), Access, Share, 0, Creation, Flags, 0);
			if (file == INVALID_HANDLE_VALUE)
				throw FileAccessException();
			return file;
		}
		void CreatePipe(handle* pipe_in, handle* pipe_out)
		{
			if (!::CreatePipe(pipe_out, pipe_in, 0, 0))
				throw Exception();
		}
		handle GetStandartOutput()
		{
			return GetStdHandle(STD_OUTPUT_HANDLE);
		}
		handle GetStandartInput()
		{
			return GetStdHandle(STD_INPUT_HANDLE);
		}
		handle GetStandartError()
		{
			return GetStdHandle(STD_ERROR_HANDLE);
		}
		void SetStandartOutput(handle file)
		{
			SetStdHandle(STD_OUTPUT_HANDLE, file);
		}
		void SetStandartInput(handle file)
		{
			SetStdHandle(STD_INPUT_HANDLE, file);
		}
		void SetStandartError(handle file)
		{
			SetStdHandle(STD_ERROR_HANDLE, file);
		}
		void CloseFile(handle file)
		{
			CloseHandle(file);
		}
		void Flush(handle file)
		{
			if (!FlushFileBuffers(file))
				throw FileAccessException();
		}
		UInt64 GetFileSize(handle file)
		{
			LARGE_INTEGER v;
			if (!GetFileSizeEx(file, &v))
				throw FileAccessException();
			return UInt64(v.QuadPart);
		}
		void MoveFile(const string& from, const string& to)
		{
			if (!::MoveFileW(NormalizePath(from).CStr(), NormalizePath(to).CStr()))
				throw FileAccessException();
		}
		bool FileExists(const string& path)
		{
			handle file = ::CreateFileW(NormalizePath(path).CStr(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (file == INVALID_HANDLE_VALUE)
				return false;
			CloseHandle(file);
			return true;
		}
		void ReadFile(handle file, void* to, UInt32 amount)
		{
			DWORD Read;
			if (!::ReadFile(file, to, amount, &Read, 0))
				throw FileAccessException();
			if (Read != amount)
				throw FileReadEndOfFileException(Read);
		}
		void WriteFile(handle file, const void* data, UInt32 amount)
		{
			DWORD Written;
			if (!::WriteFile(file, data, amount, &Written, 0))
				throw FileAccessException();
			if (Written != amount)
				throw FileAccessException();
		}
		Int64 Seek(handle file, Int64 position, SeekOrigin origin)
		{
			LARGE_INTEGER set, get;
			set.QuadPart = position;
			DWORD org = FILE_BEGIN;
			if (origin == SeekOrigin::Current)
				org = FILE_CURRENT;
			else if (origin == SeekOrigin::End)
				org = FILE_END;
			if (!SetFilePointerEx(file, set, &get, org))
				throw FileAccessException();
			return get.QuadPart;
		}
		void SetFileSize(handle file, UInt64 size)
		{
			LARGE_INTEGER pos, len;
			len.QuadPart = Int64(size);
			if (!SetFilePointerEx(file, { 0, 0 }, &pos, FILE_CURRENT))
				throw FileAccessException();
			if (!SetFilePointerEx(file, len, 0, FILE_BEGIN))
				throw FileAccessException();
			if (!SetEndOfFile(file))
			{
				SetFilePointerEx(file, pos, 0, FILE_BEGIN);
				throw FileAccessException();
			}
			SetFilePointerEx(file, pos, 0, FILE_BEGIN);
		}
		void RemoveFile(const string& path)
		{
			if (!DeleteFileW(NormalizePath(path).CStr()))
				throw FileAccessException();
		}
		void SetCurrentDirectory(const string& path)
		{
			if (!::SetCurrentDirectoryW(ExpandPath(path).CStr()))
				throw FileAccessException();
		}
		string GetCurrentDirectory()
		{
			DynamicString exp;
			exp.Resize(0x1000);
			do
			{
				auto result = ::GetCurrentDirectoryW(exp.GetLength(), exp.CStr());
				if (!result)
					throw Exception();
				if (result > UInt32(exp.GetLength()))
					exp.Resize(result);
				else
					break;
			} while (true);
			return exp.ToString();
		}
	}
}
