#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

class PCB
{
private:
	Semaphore *joinsem;
	Semaphore *exitsem;
	Semaphore *multex;

	Thread *thread;
	char filename[32];

	int exitcode;
	int numwait;

public:
	// semaphore cho quá trình join
	// semaphore cho quá trình exit
	// semaphore cho quá trình truy xuất đọc quyền
	// số tiến trình đã join
	int parentID;
	// kiem tra xem co phai tien trinh background ko
	bool isBG;
	// ID của tiến trình cha
	PCB();
	// contructor
	PCB(int id);
	// deconstructor
	~PCB();
	// nạp chương trình có tên lưu trong biến filename và processID là pid
	int Exec(char *filename, int pid);
	// Tạo 1 thread mới có tên là filename và process là pid
	int GetID();
	int GetNumWait(); // Trả về ProcessID của tiến trình gọi thực hiện
	// Trả về số lượng tiến trình chờ
	void IncNumWait();
	void DecNumWait(); // Tăng số tiến trình chờ
	// Giảm số tiến trình chờ
	void JoinWait();
	void ExitWait();
	void JoinRelease();
	void ExitRelease();
	void SetExitCode(int ec);
	int GetExitCode();
	void SetFileName(char *fn);
	char *GetFileName();
};

#endif