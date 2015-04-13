//! ==========================================================================================
//! SimpleScene.cpp - GoddamnEngine 01 Sample.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! ==========================================================================================

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/Symbol.h>
#include <GoddamnEngine/Core/Object/Reflection.h>
#include <GoddamnEngine/Core/Math/Matrix4x4.h>

#include <GoddamnEngine/Core/Platform/Application.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Atomics.h>
#include <GoddamnEngine/Core/Platform/Threading.h>
#include <GoddamnEngine/Core/Platform/OutputDevice.h>
#include <GoddamnEngine/Core/Platform/SplashScreen.h>

GD_USING_NAMESPACE;

#if 1
struct Component : public IReflectable { };

$interface(Transform $extends Component)
private:
	struct ModificationMask {
		UInt32 static const s_Zero            = 0;
		UInt32 static const s_ParentChanged   = GD_BIT(0);
		UInt32 static const s_PositionChanged = GD_BIT(1);
		UInt32 static const s_RotationChanged = GD_BIT(2);
		UInt32 static const s_ScaleChanged    = GD_BIT(3);
	};

	mutable UInt32    m_ModificationMask;
	mutable Matrix4x4 m_TransformMatrix;
	Transform const*  m_ParentTransform;
	Vector3Fast       m_Position;
	Quaternion        m_Rotation;
	Vector3Fast       m_Scale;

public:

	// ...

	//! @brief Pointer to the parent transform of the object.
	$synthesize_property_tagged(HideInEditor, ParentTransform $as decltype(m_ParentTransform))
		$getter $synthesize_generic_getter(m_ParentTransform);
		$setter {
			$self->m_ModificationMask |= ModificationMask::s_ParentChanged;
			$self->m_ParentTransform = Value;
		}
	$end;

	$synthesize_property_tagged(HideInEditor, TransformMatrix $as decltype(m_TransformMatrix))
		$getter {
			if ($self->m_ModificationMask != ModificationMask::s_Zero) {
				Matrix4x4 ParentTransformMatrix = $self->m_ParentTransform->GetTransformMatrix();
				ParentTransformMatrix.Translate($self->m_Position);
				ParentTransformMatrix.Rotate($self->m_Rotation);
				ParentTransformMatrix.Scale($self->m_Scale);
				$self->m_ModificationMask = ModificationMask::s_Zero;
			}
			return $self->m_TransformMatrix;
		}
	$end;

	$synthesize_property_tagged(Category=Transformation, Position $as decltype(m_Position))
		$getter $synthesize_generic_getter(m_Position);
		$setter {
			$self->m_ModificationMask |= ModificationMask::s_PositionChanged;
			$self->m_Position = $value;
		}
	$end;

	$synthesize_property_tagged(HideInEditor, Rotation $as decltype(m_Rotation))
		$getter $synthesize_generic_getter(m_Rotation);
		$setter {
			$self->m_ModificationMask |= ModificationMask::s_RotationChanged;
			$self->m_Rotation = $value;
		}
	$end;

	$synthesize_property_tagged(NotSerializable Category=Transformation ShowName=Rotation, RotationDegrees $as Vector3Fast)
		$getter { return $self->GetRotation().GetEulerDegrees();	}
		$setter { $self->SetRotation(Quaternion().SetEulerDegrees(Value)); }
	$end;

	$synthesize_property_tagged(Category=Transformation, Scale $as decltype(m_Scale))
		$getter $synthesize_generic_getter(m_Scale);
		$setter {
			$self->m_ModificationMask |= ModificationMask::s_ScaleChanged;
			$self->m_Scale = $value;
		}
	$end;
$end;
#endif	// if 0

int main(int const CommandLineArgsCount, char const* const* const CommandLineArgsList)
{
	Application App;
	App.InitializeObject();

	OutputDeviceConsole Debug;
	Debug.ShowConsole(true);
	Debug.LogError("HUi");

	Symbol a(L"HUI");
	printf("x%p", &a);

	SplashScreen* SS = new SplashScreen();

	SplashScreen::InitializeObject();

	while (true) { 
		
	}

	Transform* T = GD_NEW(Transform);
	ReflectionClass const* TransformClass = Transform::GetClassType();
	ReflectionProperty const* TransformPositionPr = TransformClass->ClassFirstProperty->PropertyNext->PropertyNext;
	ReflectionProperty const* TransformScalePr = TransformPositionPr->PropertyNext->PropertyNext->PropertyNext;

	Map<int, int> SM;

	Vector3Fast Pos;
	T->SetPosition(Vector3Fast(100.0f));
	TransformPositionPr->GetValueViaCopying(T, &Pos);
	TransformScalePr->SetValueViaCopying(T, &Pos);
	GD_DELETE(T);


	// Place your code here!
	GD_DEBUG_ASSERT(false, "Hello from code of the GoddamnEngine Sample!");
}
