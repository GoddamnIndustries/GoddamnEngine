#include "BinaryLoader.h"

using namespace Engine::Streaming;

namespace Engine
{
	namespace UI
	{
		namespace Loader
		{
			namespace BinaryLoader
			{
				namespace Format
				{
					ENGINE_PACKED_STRUCTURE
					struct Header
					{
						UInt8 Signature[8];
						UInt32 Version;
						UInt32 DataOffset;
						UInt32 DataSize;
						UInt32 TableOffset;
					};
					struct Table
					{
						UInt32 TextureOffset;
						Int32 TextureCount;
						UInt32 FontOffset;
						Int32 FontCount;
						UInt32 ApplicationOffset;
						Int32 ApplicationCount;
						UInt32 DialogOffset;
						Int32 DialogCount;
					};
					struct Texture
					{
						UInt32 NameOffset;
						UInt32 VersionOffset10;
						UInt32 VersionSize10;
						UInt32 VersionOffset15;
						UInt32 VersionSize15;
						UInt32 VersionOffset20;
						UInt32 VersionSize20;
					};
					struct Font
					{
						UInt32 NameOffset;
						UInt32 FaceOffset;
						UInt32 HeightA;
						Float64 HeightZ;
						UInt32 Weight;
						UInt32 Flags;
					};
					struct Application
					{
						UInt32 NameOffset;
						UInt32 RootOffset;
					};
					typedef Application Dialog;
					struct Template4
					{
						UInt32 Value;
						UInt32 Parameter;
					};
					struct Template8
					{
						Float64 Value;
						UInt32 Parameter;
					};
					struct Coordinate
					{
						Template4 A;
						Template8 Z;
						Template8 W;
					};
					struct Rectangle
					{
						Coordinate Left;
						Coordinate Top;
						Coordinate Right;
						Coordinate Bottom;
					};
					struct GradientColor
					{
						Template4 Color;
						Template8 Position;
					};
					struct ShapeBase
					{
						UInt32 Type;
						Rectangle Position;
					};
					struct ShapeFrame
					{
						ShapeBase Base;
						Int32 ChildrenCount;
						UInt32 ChildrenOffset[1];
					};
					struct ShapeBar
					{
						ShapeBase Base;
						UInt32 Horizontal;
						Int32 ColorsCount;
						UInt32 ColorsOffset[1];
					};
					struct ShapeTexture
					{
						ShapeBase Base;
						UInt32 TextureMode;
						Template4 TextureName;
						Rectangle TextureFrom;
					};
					struct ShapeText
					{
						ShapeBase Base;
						Template4 FontName;
						Template4 Color;
						Template4 Text;
						UInt32 DrawMode;
					};
					struct SmallCoordinate
					{
						Float64 W;
						Float64 Z;
						UInt32 A;
						UInt32 unused;
					};
					struct SmallRectangle
					{
						SmallCoordinate Left, Top, Right, Bottom;
					};
					struct ControlBase
					{
						UInt32 ClassName;
						SmallRectangle Position;
						Int32 SettersCount;
						UInt32 SettersOffset[1];
					};
					struct ControlTail
					{
						Int32 ChildrenCount;
						UInt32 ChildrenOffset[1];
					};
					struct Setter
					{
						UInt32 Name;
					};
					struct SetterBig
					{
						UInt32 Name;
						SmallRectangle Value;
					};
					struct SetterMedium
					{
						UInt32 Name;
						SmallCoordinate Value;
					};
					struct SetterSmall
					{
						UInt32 Name;
						UInt32 Value;
					};
					ENGINE_END_PACKED_STRUCTURE
				}
				using namespace Format;

				void LoadTexture(Texture* Source, UInt8* Data, UInt32 Size, IResourceLoader* ResourceLoader, InterfaceTemplate& Result)
				{
					SafePointer<Stream> TextureSource;
					if (Zoom > 1.75)
					{
						if (Source->VersionSize20)
						{
							TextureSource.operator=(gd_new MemoryStream(Data + Source->VersionOffset20, Source->VersionSize20));
						}
						else if (Source->VersionSize15)
						{
							TextureSource.operator=(gd_new MemoryStream(Data + Source->VersionOffset15, Source->VersionSize15));
						}
						else if (Source->VersionSize10)
						{
							TextureSource.operator=(gd_new MemoryStream(Data + Source->VersionOffset10, Source->VersionSize10));
						}
					}
					else if (Zoom > 1.25)
					{
						if (Source->VersionSize15)
						{
							TextureSource.operator=(gd_new MemoryStream(Data + Source->VersionOffset15, Source->VersionSize15));
						}
						else if (Source->VersionSize10)
						{
							TextureSource.operator=(gd_new MemoryStream(Data + Source->VersionOffset10, Source->VersionSize10));
						}
					}
					else
					{
						if (Source->VersionSize10)
						{
							TextureSource.operator=(gd_new MemoryStream(Data + Source->VersionOffset10, Source->VersionSize10));
						}
					}
					if (TextureSource.Get())
					{
						string Name = string((WideCStr)(Data + Source->NameOffset) GD_STUBBED_UTF16);
						if (Result.Texture.ElementPresent(Name))
						{
							ResourceLoader->ReloadTexture(Result.Texture[Name], TextureSource);
						}
						else
						{
							SafePointer<ITexture> New = ResourceLoader->LoadTexture(TextureSource);
							if (New.Get())
							{
								Result.Texture.InsertLast(Name, New);
							}
							else
								throw InvalidFormatException();
						}
					}
				}
				void LoadFont(Font* Source, UInt8* Data, UInt32 Size, IResourceLoader* ResourceLoader, InterfaceTemplate& Result)
				{
					string Name = string((WideCStr)(Data + Source->NameOffset) GD_STUBBED_UTF16);
					if (Result.Font.ElementPresent(Name))
					{
						ResourceLoader->ReloadFont(Result.Font[Name]);
					}
					else
					{
						string FontFace = string((WideCStr)(Data + Source->FaceOffset) GD_STUBBED_UTF16);
						SafePointer<IFont> New = ResourceLoader->LoadFont(FontFace, Source->HeightA + Int32(Source->HeightZ * Zoom), Source->Weight, (Source->Flags & 0x01) != 0,
						    (Source->Flags & 0x02) != 0, (Source->Flags & 0x04) != 0);
						if (New.Get())
						{
							Result.Font.InsertLast(Name, New);
						}
						else
							throw InvalidFormatException();
					}
				}

				Template::BasicTemplate<Int32> LoadIntegerTemplate(Template4* Source, const UInt8* Data, UInt32 Size)
				{
					string Argument = string((WideCStr)(Data + Source->Parameter) GD_STUBBED_UTF16);
					if (Argument.GetLength())
						return Template::BasicTemplate<Int32>::Undefined(Argument);
					else
						return Template::BasicTemplate<Int32>(Source->Value);
				}
				Template::BasicTemplate<Float64> LoadFloatTemplate(Template8* Source, const UInt8* Data, UInt32 Size)
				{
					string Argument = string((WideCStr)(Data + Source->Parameter) GD_STUBBED_UTF16);
					if (Argument.GetLength())
						return Template::BasicTemplate<Float64>::Undefined(Argument);
					else
						return Template::BasicTemplate<Float64>(Source->Value);
				}
				Template::BasicTemplate<Color> LoadColorTemplate(Template4* Source, const UInt8* Data, UInt32 Size)
				{
					string Argument = string((WideCStr)(Data + Source->Parameter) GD_STUBBED_UTF16);
					if (Argument.GetLength())
						return Template::BasicTemplate<Color>::Undefined(Argument);
					else
						return Template::BasicTemplate<Color>(Color(Source->Value));
				}
				Template::BasicTemplate<string> LoadTextTemplate(Template4* Source, const UInt8* Data, UInt32 Size)
				{
					string Argument = string((WideCStr)(Data + Source->Parameter) GD_STUBBED_UTF16);
					if (Argument.GetLength())
						return Template::BasicTemplate<string>::Undefined(Argument);
					else
						return Template::BasicTemplate<string>(string((WideCStr)(Data + Source->Value) GD_STUBBED_UTF16));
				}
				Template::ObjectTemplate<ITexture> LoadTextureTemplate(
				    Template4* Source, const UInt8* Data, UInt32 Size, IResourceResolver* ResourceResolver, InterfaceTemplate& Result)
				{
					string Argument = string((WideCStr)(Data + Source->Parameter) GD_STUBBED_UTF16);
					if (!Argument.GetLength())
					{
						string Resource = string((WideCStr)(Data + Source->Value) GD_STUBBED_UTF16);
						if (Resource.GetLength())
						{
							SafePointer<ITexture> Value = Result.Texture[Resource];
							if (!Value.Get() && ResourceResolver)
								Value.operator=(ResourceResolver->GetTexture(Resource));
							else if (Value.Get())
								Value->AddRef();
							return Template::ObjectTemplate<ITexture>(Value);
						}
						else
							return Template::ObjectTemplate<ITexture>(0);
					}
					else
						return Template::ObjectTemplate<ITexture>::Undefined(Argument);
				}
				Template::ObjectTemplate<IFont> LoadFontTemplate(Template4* Source, const UInt8* Data, UInt32 Size, IResourceResolver* ResourceResolver, InterfaceTemplate& Result)
				{
					string Argument = string((WideCStr)(Data + Source->Parameter) GD_STUBBED_UTF16);
					if (!Argument.GetLength())
					{
						string Resource = string((WideCStr)(Data + Source->Value) GD_STUBBED_UTF16);
						if (Resource.GetLength())
						{
							SafePointer<IFont> Value = Result.Font[Resource];
							if (!Value.Get() && ResourceResolver)
								Value.operator=(ResourceResolver->GetFont(Resource));
							else if (Value.Get())
								Value->AddRef();
							return Template::ObjectTemplate<IFont>(Value);
						}
						else
							return Template::ObjectTemplate<IFont>(0);
					}
					else
						return Template::ObjectTemplate<IFont>::Undefined(Argument);
				}
				Template::Coordinate LoadCoordinateTemplate(Coordinate* Source, const UInt8* Data, UInt32 Size)
				{
					Template::Coordinate Result;
					Result.Absolute = LoadIntegerTemplate(&Source->A, Data, Size);
					Result.Zoom = LoadFloatTemplate(&Source->Z, Data, Size);
					Result.Anchor = LoadFloatTemplate(&Source->W, Data, Size);
					return Result;
				}
				Template::Rectangle LoadRectangleTemplate(Rectangle* Source, const UInt8* Data, UInt32 Size)
				{
					Template::Rectangle Result;
					Result.Left = LoadCoordinateTemplate(&Source->Left, Data, Size);
					Result.Top = LoadCoordinateTemplate(&Source->Top, Data, Size);
					Result.Right = LoadCoordinateTemplate(&Source->Right, Data, Size);
					Result.Bottom = LoadCoordinateTemplate(&Source->Bottom, Data, Size);
					return Result;
				}
				Template::GradientPoint LoadGradientTemplate(GradientColor* Source, const UInt8* Data, UInt32 Size)
				{
					Template::GradientPoint Result;
					Result.PointColor = LoadColorTemplate(&Source->Color, Data, Size);
					Result.Position = LoadFloatTemplate(&Source->Position, Data, Size);
					return Result;
				}

				Template::Shape* LoadShape(ShapeBase* Source, UInt8* Data, UInt32 Size, IResourceResolver* ResourceResolver, InterfaceTemplate& Result)
				{
					auto Position = LoadRectangleTemplate(&Source->Position, Data, Size);
					if (Source->Type == 0)
					{
						auto Shape = reinterpret_cast<ShapeFrame*>(Source);
						SafePointer<Template::FrameShape> Template = gd_new Template::FrameShape;
						Template->Position = Position;
						Template->RenderMode = FrameShape::FrameRenderMode::Normal;
						Template->Opacity = 1.0;
						for (Int32 i = 0; i < Shape->ChildrenCount; i++)
						{
							SafePointer<Template::Shape> Entity = LoadShape(reinterpret_cast<ShapeBase*>(Data + Shape->ChildrenOffset[i]), Data, Size, ResourceResolver, Result);
							if (Entity.Get())
								Template->Children.InsertLast(Entity);
						}
						Template->AddRef();
						return Template;
					}
					else if (Source->Type == 1)
					{
						ShapeBar* Shape = reinterpret_cast<ShapeBar*>(Source);
						SafePointer<Template::BarShape> Template = gd_new Template::BarShape;
						Template->Position = Position;
						Template->GradientAngle = Shape->Horizontal ? 0.0 : (-ENGINE_PI / 2.0);
						for (Int32 i = 0; i < Shape->ColorsCount; i++)
						{
							Template->Gradient << LoadGradientTemplate(reinterpret_cast<GradientColor*>(Data + Shape->ColorsOffset[i]), Data, Size);
						}
						Template->AddRef();
						return Template;
					}
					else if (Source->Type == 2)
					{
						ShapeTexture* Shape = reinterpret_cast<ShapeTexture*>(Source);
						SafePointer<Template::TextureShape> Template = gd_new Template::TextureShape;
						Template->Position = Position;
						Template->Texture = LoadTextureTemplate(&Shape->TextureName, Data, Size, ResourceResolver, Result);
						Template->From = LoadRectangleTemplate(&Shape->TextureFrom, Data, Size);
						Template->RenderMode = static_cast<TextureShape::TextureRenderMode>(Shape->TextureMode);
						Template->AddRef();
						return Template;
					}
					else if (Source->Type == 3)
					{
						ShapeText* Shape = reinterpret_cast<ShapeText*>(Source);
						SafePointer<Template::TextShape> Template = gd_new Template::TextShape;
						Template->Position = Position;
						Template->Font = LoadFontTemplate(&Shape->FontName, Data, Size, ResourceResolver, Result);
						Template->Text = LoadTextTemplate(&Shape->Text, Data, Size);
						Template->TextColor = LoadColorTemplate(&Shape->Color, Data, Size);
						if ((Shape->DrawMode & 0x3) == 2)
							Template->HorizontalAlign = TextShape::TextHorizontalAlign::Right;
						else if ((Shape->DrawMode & 0x3) == 1)
							Template->HorizontalAlign = TextShape::TextHorizontalAlign::Center;
						else if ((Shape->DrawMode & 0x3) == 0)
							Template->HorizontalAlign = TextShape::TextHorizontalAlign::Left;
						if ((Shape->DrawMode & 0xC) == 8)
							Template->VerticalAlign = TextShape::TextVerticalAlign::Bottom;
						else if ((Shape->DrawMode & 0xC) == 4)
							Template->VerticalAlign = TextShape::TextVerticalAlign::Center;
						else if ((Shape->DrawMode & 0xC) == 0)
							Template->VerticalAlign = TextShape::TextVerticalAlign::Top;
						Template->AddRef();
						return Template;
					}
					return 0;
				}
				void LoadApplication(Application* Source, UInt8* Data, UInt32 Size, IResourceResolver* ResourceResolver, InterfaceTemplate& Result)
				{
					string Name = string((WideCStr)(Data + Source->NameOffset) GD_STUBBED_UTF16);
					if (!Result.Application.ElementPresent(Name))
					{
						Result.Application.InsertLast(Name, LoadShape(reinterpret_cast<ShapeBase*>(Data + Source->RootOffset), Data, Size, ResourceResolver, Result));
					}
				}

				Template::ControlTemplate* LoadControl(ControlBase* Source, UInt8* Data, UInt32 Size, IResourceResolver* ResourceResolver, InterfaceTemplate& Result)
				{
					string ClassName = string((WideCStr)(Data + Source->ClassName) GD_STUBBED_UTF16);
					auto ControlPropertyBase = Template::Controls::CreateControlByClass(ClassName);
					Reflection::PropertyZeroInitializer Initializer;
					ControlPropertyBase->EnumerateProperties(Initializer);
					if (ControlPropertyBase)
					{
						SafePointer<Template::ControlTemplate> Template = gd_new Template::ControlTemplate(ControlPropertyBase);
						Template->Properties->ControlPosition = UI::Rectangle(UI::Coordinate(Source->Position.Left.A, Source->Position.Left.Z, Source->Position.Left.W),
						    UI::Coordinate(Source->Position.Top.A, Source->Position.Top.Z, Source->Position.Top.W),
						    UI::Coordinate(Source->Position.Right.A, Source->Position.Right.Z, Source->Position.Right.W),
						    UI::Coordinate(Source->Position.Bottom.A, Source->Position.Bottom.Z, Source->Position.Bottom.W));
						for (Int32 i = 0; i < Source->SettersCount; i++)
						{
							auto PropertySetter = reinterpret_cast<Setter*>(Data + Source->SettersOffset[i]);
							auto Property = Template->Properties->GetProperty(string((WideCStr)(Data + PropertySetter->Name) GD_STUBBED_UTF16));
							if (Property.Type != Reflection::PropertyType::Unknown)
							{
								if (Property.Type == Reflection::PropertyType::Boolean || Property.Type == Reflection::PropertyType::Color
								    || Property.Type == Reflection::PropertyType::String || Property.Type == Reflection::PropertyType::Texture
								    || Property.Type == Reflection::PropertyType::Font || Property.Type == Reflection::PropertyType::Application
								    || Property.Type == Reflection::PropertyType::Dialog)
								{
									auto Ex = reinterpret_cast<SetterSmall*>(PropertySetter);
									if (Property.Type == Reflection::PropertyType::Boolean)
									{
										Property.Set<bool>(Ex->Value != 0);
									}
									else if (Property.Type == Reflection::PropertyType::Color)
									{
										Property.Set<Color>(Ex->Value);
									}
									else
									{
										string Resource = string((WideCStr)(Data + Ex->Value) GD_STUBBED_UTF16);
										if (Property.Type == Reflection::PropertyType::String)
										{
											Property.Set<string>(Resource);
										}
										else if (Property.Type == Reflection::PropertyType::Texture)
										{
											if (Resource.GetLength())
											{
												SafePointer<ITexture> Texture = Result.Texture[Resource];
												if (!Texture.Get() && ResourceResolver)
													Texture.operator=(ResourceResolver->GetTexture(Resource));
												else if (Texture.Get())
													Texture->AddRef();
												Property.Get<SafePointer<ITexture>>() = Texture;
											}
										}
										else if (Property.Type == Reflection::PropertyType::Font)
										{
											if (Resource.GetLength())
											{
												SafePointer<IFont> Font = Result.Font[Resource];
												if (!Font.Get() && ResourceResolver)
													Font.operator=(ResourceResolver->GetFont(Resource));
												else if (Font.Get())
													Font->AddRef();
												Property.Get<SafePointer<IFont>>() = Font;
											}
										}
										else if (Property.Type == Reflection::PropertyType::Application)
										{
											if (Resource.GetLength())
											{
												SafePointer<Template::Shape> Shape = Result.Application[Resource];
												if (!Shape.Get() && ResourceResolver)
													Shape.operator=(ResourceResolver->GetApplication(Resource));
												else if (Shape.Get())
													Shape->AddRef();
												Property.Get<SafePointer<Template::Shape>>() = Shape;
											}
										}
										else if (Property.Type == Reflection::PropertyType::Dialog)
										{
											if (Resource.GetLength())
											{
												SafePointer<Template::ControlTemplate> Control = Result.Dialog[Resource];
												if (!Control.Get() && ResourceResolver)
													Control.operator=(ResourceResolver->GetDialog(Resource));
												else if (Control.Get())
													Control->AddRef();
												Property.Get<SafePointer<Template::ControlTemplate>>() = Control;
											}
										}
									}
								}
								else if (Property.Type == Reflection::PropertyType::Integer || Property.Type == Reflection::PropertyType::Double)
								{
									auto Ex = reinterpret_cast<SetterMedium*>(PropertySetter);
									if (Property.Type == Reflection::PropertyType::Integer)
									{
										Property.Set<Int32>(Ex->Value.A + Int32(Ex->Value.Z * Zoom));
									}
									else if (Property.Type == Reflection::PropertyType::Double)
									{
										Property.Set<Float64>(Ex->Value.Z);
									}
								}
								else if (Property.Type == Reflection::PropertyType::Rectangle)
								{
									auto Ex = reinterpret_cast<SetterBig*>(PropertySetter);
									Property.Set<UI::Rectangle>(UI::Rectangle(UI::Coordinate(Ex->Value.Left.A, Ex->Value.Left.Z, Ex->Value.Left.W),
									    UI::Coordinate(Ex->Value.Top.A, Ex->Value.Top.Z, Ex->Value.Top.W), UI::Coordinate(Ex->Value.Right.A, Ex->Value.Right.Z, Ex->Value.Right.W),
									    UI::Coordinate(Ex->Value.Bottom.A, Ex->Value.Bottom.Z, Ex->Value.Bottom.W)));
								}
							}
						}
						ControlTail* Tail = reinterpret_cast<ControlTail*>(reinterpret_cast<UInt8*>(Source) + 8 + sizeof(SmallRectangle) + 4 * Source->SettersCount);
						for (Int32 i = 0; i < Tail->ChildrenCount; i++)
						{
							SafePointer<Template::ControlTemplate> Entity
							    = LoadControl(reinterpret_cast<ControlBase*>(Data + Tail->ChildrenOffset[i]), Data, Size, ResourceResolver, Result);
							if (Entity.Get())
								Template->Children.InsertLast(Entity);
						}
						Template->AddRef();
						return Template;
					}
					else
						return 0;
				}
				void LoadDialog(Dialog* Source, UInt8* Data, UInt32 Size, IResourceResolver* ResourceResolver, InterfaceTemplate& Result)
				{
					string Name = string((WideCStr)(Data + Source->NameOffset) GD_STUBBED_UTF16);
					if (!Result.Dialog.ElementPresent(Name))
					{
						Result.Dialog.InsertLast(Name, LoadControl(reinterpret_cast<ControlBase*>(Data + Source->RootOffset), Data, Size, ResourceResolver, Result));
					}
				}

				void BuildInterface(InterfaceTemplate& Template, Table* Source, UInt8* Data, UInt32 Size, IResourceLoader* ResourceLoader, IResourceResolver* ResourceResolver)
				{
					for (Int32 i = 0; i < Source->TextureCount; i++)
					{
						LoadTexture(reinterpret_cast<Texture*>(Data + Source->TextureOffset + i * sizeof(Texture)), Data, Size, ResourceLoader, Template);
					}
					for (Int32 i = 0; i < Source->FontCount; i++)
					{
						LoadFont(reinterpret_cast<Font*>(Data + Source->FontOffset + i * sizeof(Font)), Data, Size, ResourceLoader, Template);
					}
					for (Int32 i = 0; i < Source->ApplicationCount; i++)
					{
						LoadApplication(reinterpret_cast<Application*>(Data + Source->ApplicationOffset + i * sizeof(Application)), Data, Size, ResourceResolver, Template);
					}
					for (Int32 i = 0; i < Source->DialogCount; i++)
					{
						LoadDialog(reinterpret_cast<Dialog*>(Data + Source->DialogOffset + i * sizeof(Dialog)), Data, Size, ResourceResolver, Template);
					}
				}
			}
			void LoadUserInterfaceFromBinary(InterfaceTemplate& Template, Streaming::Stream* Source, IResourceLoader* ResourceLoader, IResourceResolver* ResourceResolver)
			{
				auto base = Source->Seek(0, Current);
				auto length = Source->Length() - base;
				if (length < sizeof(BinaryLoader::Header))
					throw InvalidFormatException();
				BinaryLoader::Header hdr;
				Source->Read(&hdr, sizeof(hdr));
				if (MemoryCompare(&hdr.Signature, "eusrint\0", 8) != 0)
					throw InvalidFormatException();
				if (hdr.Version != 0)
					throw InvalidFormatException();
				if (hdr.DataOffset > length)
					throw InvalidFormatException();
				if (hdr.DataOffset + hdr.DataSize > length)
					throw InvalidFormatException();
				if (hdr.TableOffset + sizeof(BinaryLoader::Table) > hdr.DataSize)
					throw InvalidFormatException();
				UInt8* data = gd_new UInt8[hdr.DataSize];
				if (!data)
					throw OutOfMemoryException();
				try
				{
					Source->Seek(base + hdr.DataOffset, SeekOrigin::Begin);
					Source->Read(data, hdr.DataSize);
					BinaryLoader::BuildInterface(Template, reinterpret_cast<BinaryLoader::Table*>(data + hdr.TableOffset), data, hdr.DataSize, ResourceLoader, ResourceResolver);
				}
				catch (...)
				{
					gd_delete[] data;
					throw;
				}
				gd_delete[] data;
			}
		}
	}
}
