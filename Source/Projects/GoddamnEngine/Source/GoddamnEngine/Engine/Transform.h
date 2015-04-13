
class Transform;

$interface(Transform $extends Component)
private:
	struct ModificationMask {
		UInt32 static const s_Zero            = 0;
		UInt32 static const s_ParentChanged   = GD_BIT(0);
		UInt32 static const s_PositionChanged = GD_BIT(1);
		UInt32 static const s_RotationChanged = GD_BIT(2);
		UInt32 static const s_ScaleChanged    = GD_BIT(3);
	};

	UInt32      m_ModificationMask;
	Matrix4x4   m_TransformMatrix;
	Transform*  m_ParentTransform;
	Vector3Fast m_Position;
	Quaternion  m_Rotation;
	Vector3Fast m_Scale;

public:

	// ...

	$synthesize_property_raw(TransformMatrix $as decltype(m_TransformMatrix))
		$getter {
			if (Self->m_ModificationMask != ModificationMask::s_Zero) {
				Matrix4x4 ParentTransformMatrix = Self->m_ParentTransform->TransformMatrix;
				ParentTransformMatrix.Translate(Self->m_Position);
				ParentTransformMatrix.Rotate(Self->m_Rotation);
				ParentTransformMatrix.Scale(Self->m_Scale);
				Self->m_ModificationMask = ModificationMask::s_Zero;
			}
			return Self->m_TransformMatrix;
		}
		$setter $none;
	$end;

	$synthesize_property(ParentTransform $as decltype(ParentTransform))
		$getter $generic_getter(m_ParentTransform);
		$setter {
			Self->m_ModificationMask |= ModificationMask::s_ParentChanged;
			Self->m_ParentTransform = $value;
		}
	$end;

	$synthesize_property(Position $as decltype(m_Position))
		$getter $generic_getter(m_Position);
		$setter {
			Self->m_ModificationMask |= ModificationMask::s_PositionChanged;
			Self->m_Position = $value;
		};
	$end;

	$synthesize_property(Rotation $as delctype(m_Rotation))
		$getter $generic_getter(m_Rotation);
		$setter {
			Self->m_ModificationMask |= ModificationMask::s_RotationChanged;
			Self->m_Rotation = $value;
		};
	$end;

	$synthesize_property(Scale $as delctype(m_Scale))
		$getter $generic_getter(m_Scale);
		$setter {
			Self->m_ModificationMask |= ModificationMask::s_ScaleChanged;
			Self->m_Scale = $value;
		};
	$end;
$end;
