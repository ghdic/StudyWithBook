#ifndef TASKH
#define TASKH

class CTask;
class CTaskList;
class CTaskIter;


//==============================================================
// Task
class CTask {
friend CTaskList;
friend CTaskIter;

	// Task ����Ʈ�� ������
	CTaskList* TaskList;

	// ��,�� Task�� ������
	CTask *Prev, *Next;

	// Task ����Ʈ�� �������� ���� new, delete�� ��ȿȭ �Ѵ�.
	void* operator new(size_t t) {}
	void operator delete(void* p) {}

protected:
	
	// new, delete�� ó�� :
	// ������ new, delete �����ڸ� ���� Ŭ�������� �����Ѵ�.
	static void* operator_new(size_t t, CTaskList* task_list);
	static void operator_delete(void* p, CTaskList* task_list);

public:
	CTask(CTaskList* task_list);
	virtual ~CTask();
};


//==============================================================
// Task ����Ʈ
class CTaskList {
friend CTask;
friend CTaskIter;

	// Active task ����Ʈ, Free task ����Ʈ
	CTask *ActiveTask, *FreeTask;

	// Task�� �ִ� ũ��, Task�� �ִ� ��
	int MaxTaskSize, MaxNumTask;
	
	// Free task�� ��
	int NumFreeTask;

public:

	// ������
	CTaskList(int max_task_size, int max_num_task);
	
	// Task�� ��
	int GetNumFreeTask() { return NumFreeTask; }
	int GetNumActiveTask() { return MaxNumTask-NumFreeTask; }

	// ��� Task�� ����
	void DeleteTask();
};


//==============================================================
// Task iterator
class CTaskIter {
protected:
	CTaskList* TaskList;
	CTask* Task;
public:

	// ������
	inline CTaskIter(CTaskList* task_list)
	: TaskList(task_list), Task(task_list->ActiveTask)
	{}

	// ���� ��Ұ� ���� ��� true
	inline bool HasNext() {
		return Task->Next!=TaskList->ActiveTask;
	}

	// ���� ��Ҹ� ��ȯ
	inline CTask* Next() {
		return Task=Task->Next;
	}

	// ������ ��ȯ�� ��Ҹ� ����
	inline void Remove() {
		Task=Task->Prev;
		delete Task->Next;
	}
};


#endif

