#define GD_DLL_IMPORT 1

#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/Class.h>
#include <GoddamnEngine/Core/Object/ObjectVisitor.h>
#include <GoddamnEngine/Core/System/Threading/JobManager.h>
#include <GoddamnEngine/Core/Object/Serialization/Serialization.h>
#include <GoddamnEngine/Core/System/IO/FileStream.h>
using namespace GD_NAMESPACE;

class MyObject : public SerializableObject
{
	GD_DECLARE_OBJECT(MyObject, SerializableObject, "");

public:

	/*!
	 * First property. 
	 */
	GD_PROPERTY(public, Int64, m_FirstProperty, PFNone) = -13;

	/*!
	 * Second property. 
	 */
	GD_PROPERTY(public, String, m_SecondProperty, PFNone) = "hui";

	/*!
	 * Third property.
 	 */
	GD_PROPERTY(public, Vector<String>, m_ThirdProperty, PFNone) = { "aaa", "bb", "-", "c" };

public:
};	// class MyObject

GD_IMPLEMENT_OBJECT(GDINT, MyObject);

MyObject::~MyObject()
{
}


int main(/*Platform::Array<Platform::String^>^ args*/)
{
	JobManager::ParallelJobList jobList = {};

	clock_t tx = 0;
	for (int y = 0; y < 20; ++y)
	{
		auto const t0 = clock();

		WideString strings[] = { L"hello", L"world", L"fuck", L"everybody", L"rage" };
		for (UInt32 cnt = 0; cnt < GetLength(strings); ++cnt)
		{
			auto const& s = strings[cnt];
			GD_SUBMIT_PARALLEL_JOB2(jobList, s, cnt, 
			{
				WideString ss = s;
				for (UInt32 cnt1 = 0; cnt1 < 1000; ++cnt1)
				{
					ss += s.SubstringLen(1, s.GetLength() - 1);
					ss += s;
				}

				auto l = SharedPtr<OutputStream>(gd_new FileOutputStream(s.CStr()));
				l->Write(ss.CStr(), ss.GetLength(), sizeof(WideChar));
			});
		}

		jobList.Wait();

		auto const t1 = clock() - t0;
		tx += t1;
		printf_s("\n%d", t1);
	}
	printf_s("\n==================%d", tx / 20);

	auto i = SharedPtr<InputStream>(gd_new FileInputStream(L"./test.json"));
	auto l = SharedPtr<OutputStream>(gd_new FileOutputStream(L"out.json"));

	auto const t0 = clock();
	for (int t = 0; t < 1000; ++t)
	{
		i->Seek(0, CStdio::FseekOrigin::SeekSet);
		auto const o = Object::FindOr—reateObject(EmptyGUID, MyObject::GetStaticClass());
		auto const s = static_cast<SerializableObject*>(o.Get());
		s->DeserializeSync(i);
	}
	jobList.Wait();

	auto const t1 = clock() - t0;
	printf_s("\n%d", t1);

	/*auto const package = PackageManager::FindOrCreatePackage(L"hui");
	
	package->Save();*/

	_getc_nolock(stdin);
}
