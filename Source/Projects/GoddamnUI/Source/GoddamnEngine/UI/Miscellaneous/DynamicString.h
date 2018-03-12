#pragma once

#include "../EngineBase.h"

namespace Engine
{
#if 0
	class DynamicString : public Object
	{
		Array<widechar> data;

	public:
		DynamicString();
		explicit DynamicString(const ImmutableString& src);
		explicit DynamicString(Int32 BlockSize);
		explicit DynamicString(const ImmutableString& src, Int32 BlockSize);

		operator ImmutableString() const;
		operator widechar*();
		operator const widechar*() const;
		ImmutableString ToString() const override;
		Int32 Length() const;

		widechar CharAt(Int32 index) const;
		widechar& CharAt(Int32 index);
		widechar operator[](Int32 index) const;
		widechar& operator[](Int32 index);

		void Concatenate(const ImmutableString& str);
		void Concatenate(widechar letter);
		DynamicString& operator+=(const ImmutableString& str);
		DynamicString& operator+=(widechar letter);
		DynamicString& operator<<(const ImmutableString& str);
		DynamicString& operator<<(widechar letter);

		void Insert(const ImmutableString& str, Int32 at);
		void RemoveRange(Int32 start, Int32 amount);
		void Clear();
		void ReserveLength(Int32 length);
		Int32 ReservedLength() const;
	};
#endif
	using DynamicString = WideStringBuilder;
}
