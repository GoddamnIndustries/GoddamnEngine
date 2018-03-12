#include "StringConvert.h"

#include "Base.h"

namespace Engine
{
	struct CharReader
	{
		const UInt8* Source;
		Int32 Position, Length;
		Encoding Coding;

		bool EOF() const
		{
			return Position >= Length;
		}
		UInt32 ReadByte()
		{
			UInt8 r = *(Source + Position);
			Position++;
			return r;
		}
		UInt32 ReadWord()
		{
			UInt16 r = *reinterpret_cast<const UInt16*>(Source + Position);
			Position += 2;
			return r;
		}
		UInt32 ReadDWord()
		{
			UInt32 r = *reinterpret_cast<const UInt32*>(Source + Position);
			Position += 4;
			return r;
		}
		UInt32 ReadChar()
		{
			if (Coding == Encoding::ANSI)
				return ReadByte();
			else if (Coding == Encoding::UTF8)
			{
				UInt32 code = ReadByte();
				if (code & 0x80)
				{
					code &= 0x7F;
					if (code & 0x20)
					{
						code &= 0x1F;
						if (code & 0x10)
						{
							code &= 0x07;
							code <<= 18;
							code |= (ReadByte() & 0x3F) << 12;
							code |= (ReadByte() & 0x3F) << 6;
							code |= (ReadByte() & 0x3F);
						}
						else
						{
							code &= 0x0F;
							code <<= 12;
							code |= (ReadByte() & 0x3F) << 6;
							code |= (ReadByte() & 0x3F);
						}
					}
					else
					{
						code &= 0x1F;
						code <<= 6;
						code |= (ReadByte() & 0x3F);
					}
				}
				return code;
			}
			else if (Coding == Encoding::UTF16)
			{
				UInt32 code = ReadWord();
				if ((code & 0xFC00) == 0xD800)
				{
					code &= 0x03FF;
					code <<= 10;
					code |= (ReadWord() & 0x3FF);
					code += 0x10000;
				}
				return code;
			}
			else if (Coding == Encoding::UTF32)
				return ReadDWord();
			return 0;
		}
		static Int32 CharLength(UInt32 chr, Encoding in)
		{
			if (in == Encoding::ANSI)
				return 1;
			else if (in == Encoding::UTF32)
				return 1;
			else if (in == Encoding::UTF16)
			{
				if (chr > 0xFFFF)
					return 2;
				else
					return 1;
			}
			else if (in == Encoding::UTF8)
			{
				if (chr > 0xFFFF)
					return 4;
				else if (chr > 0x7FF)
					return 3;
				else if (chr > 0x7F)
					return 2;
				else
					return 1;
			}
			return 1;
		}
		void MeasureLength()
		{
			if (Length == -1)
			{
				Int32 pos = Position;
				UInt32 c;
				do
				{
					c = ReadChar();
				} while (c);
				Length = Position;
				Position = pos;
			}
			else
			{
				if (Coding == Encoding::UTF16)
					Length *= 2;
				else if (Coding == Encoding::UTF32)
					Length *= 4;
			}
		}
	};
	struct CharWriter
	{
		UInt8* Dest;
		Int32 Position;
		Encoding Coding;
		void WriteByte(UInt8 v)
		{
			*(Dest + Position) = v;
			Position++;
		}
		void WriteWord(UInt16 v)
		{
			*reinterpret_cast<UInt16*>(Dest + Position) = v;
			Position += 2;
		}
		void WriteDWord(UInt32 v)
		{
			*reinterpret_cast<UInt32*>(Dest + Position) = v;
			Position += 4;
		}
		void WriteChar(UInt32 chr)
		{
			if (Coding == Encoding::ANSI)
				WriteByte((UInt8)((chr < 0x100) ? chr : '?'));
			else if (Coding == Encoding::UTF8)
			{
				if (chr > 0xFFFF)
				{
					WriteByte(((chr >> 18) & 0x07) | 0xF0);
					WriteByte(((chr >> 12) & 0x3F) | 0x80);
					WriteByte(((chr >> 6) & 0x3F) | 0x80);
					WriteByte(((chr)&0x3F) | 0x80);
				}
				else if (chr > 0x7FF)
				{
					WriteByte(((chr >> 12) & 0x0F) | 0xE0);
					WriteByte(((chr >> 6) & 0x3F) | 0x80);
					WriteByte(((chr)&0x3F) | 0x80);
				}
				else if (chr > 0x7F)
				{
					WriteByte(((chr >> 6) & 0x1F) | 0xC0);
					WriteByte(((chr)&0x3F) | 0x80);
				}
				else
					WriteByte((UInt8)chr);
			}
			else if (Coding == Encoding::UTF16)
			{
				if (chr > 0xFFFF)
				{
					UInt32 s = chr - 0x10000;
					WriteWord(((s >> 10) & 0x3FF) | 0xD800);
					WriteWord((s & 0x3FF) | 0xDC00);
				}
				else
					WriteWord((UInt16)chr);
			}
			else if (Coding == Encoding::UTF32)
				WriteDWord(chr);
		}
	};
	Int32 MeasureSequenceLength(const void* Source, Int32 SourceLength, Encoding From, Encoding To)
	{
		if (From == To && SourceLength != -1)
			return SourceLength;
		Int32 len = 0;
		CharReader Reader;
		Reader.Source = reinterpret_cast<const UInt8*>(Source);
		Reader.Position = 0;
		Reader.Length = SourceLength;
		Reader.Coding = From;
		Reader.MeasureLength();
		while (!Reader.EOF())
			len += Reader.CharLength(Reader.ReadChar(), To);
		return len;
	}
	void ConvertEncoding(void* Dest, const void* Source, Int32 SourceLength, Encoding From, Encoding To)
	{
		CharReader Reader;
		Reader.Source = reinterpret_cast<const UInt8*>(Source);
		Reader.Position = 0;
		Reader.Length = SourceLength;
		Reader.Coding = From;
		Reader.MeasureLength();
		if (From == To)
		{
			MemoryCopy(Dest, Source, Reader.Length);
		}
		else
		{
			CharWriter Writer;
			Writer.Dest = reinterpret_cast<UInt8*>(Dest);
			Writer.Position = 0;
			Writer.Coding = To;
			while (!Reader.EOF())
				Writer.WriteChar(Reader.ReadChar());
		}
	}
	Int32 GetBytesPerChar(Encoding encoding)
	{
		if (encoding == Encoding::ANSI)
			return 1;
		else if (encoding == Encoding::UTF8)
			return 1;
		else if (encoding == Encoding::UTF16)
			return 2;
		else if (encoding == Encoding::UTF32)
			return 4;
		return 0;
	}
}
