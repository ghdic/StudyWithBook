#include "Task.h"
#include <stdio.h>
#include <assert.h>


//=============================================================
// Task ����Ʈ �ʱ�ȭ
CTaskList::CTaskList(int max_task_size, int max_num_task)
:	MaxTaskSize(max_task_size), MaxNumTask(max_num_task), 
	NumFreeTask(max_num_task)
{
	// Task �ʱ�ȭ�� ��ũ��
	#define TASK(INDEX) ((CTask*)(buf+max_task_size*(INDEX)))

	// Task�� �޸� Ȯ��
	char* buf=new char[max_task_size*(max_num_task+2)];

	// Active task ����Ʈ�� �ʱ�ȭ
	ActiveTask=TASK(0);
	ActiveTask->Prev=ActiveTask->Next=ActiveTask;

	// Free task ����Ʈ�� �ʱ�ȭ
	FreeTask=TASK(1);
	for (int i=1; i<max_num_task+1; i++) {
		TASK(i)->Next=TASK(i+1);
	}
	TASK(max_num_task+1)->Next=FreeTask;
}


//=============================================================
// Task ����
// Free task ����Ʈ �󿡼��� �޸� ����� �պκп� CTask�� ��ġ�Ѵ�
// ���� Ŭ������ �����ڸ� ȣ���� �Ŀ���
// �޸� ����� �պκп� CTask�� ��ġ�Ǵ� ���� �ƴϴ�.
// ����ó���� new�� �����ڷ� ������,
// ����ó�� ���� �Ҹ��ڿ� delete�� ������.

// new�� ó��
void* CTask::operator_new(size_t t, CTaskList* task_list) {

	// Ŭ���� ũ�Ⱑ �ִ� ũ�⸦ ������ ����.
	assert(t<=(size_t)task_list->MaxTaskSize);

	// Free task �� ���� ������ NULL�� ��ȯ
	if (task_list->NumFreeTask<=0) return NULL;

	// Free task�� �ϳ� ������
	CTask* task=task_list->FreeTask->Next;
	task_list->FreeTask->Next=task->Next;
	task_list->NumFreeTask--;
	
	// �����ڷ�
	return task;
}

// ������
CTask::CTask(CTaskList* task_list)
:	TaskList(task_list)
{

	// Task�� Active task ����Ʈ�� �ִ´�.
	Prev=task_list->ActiveTask->Prev;
	Next=task_list->ActiveTask;
	Prev->Next=Next->Prev=this;
}


//=============================================================
// Task ����

// �Ҹ���
CTask::~CTask() {

	// Task�� Active task ����Ʈ���� �����Ѵ�
	Prev->Next=Next;
	Next->Prev=Prev;
}

// delete�� ó��
void CTask::operator_delete(void* p, CTaskList* task_list) {

	// Task
	CTask* task=(CTask*)p;

	// Task�� Free task ����Ʈ�� �ִ´�.
	task->Next=task_list->FreeTask->Next;
	task_list->FreeTask->Next=task;
	task_list->NumFreeTask++;

	// Free task ���� �ִ� task ���� ������ ����.
	// (delete�� �ߺ�ȣ�� ��)
	assert(task_list->NumFreeTask<=task_list->MaxNumTask);
}


//=============================================================
// ��� Task�� ����
void CTaskList::DeleteTask() {
	for (CTaskIter i(this); i.HasNext(); i.Next(), i.Remove()) ;
}
