/**************************************************************************/
/*                         This file is part of:                          */
/*                      Marisa's Atelier of Danmaku                       */
/*                              2024/11/19                                */
/**************************************************************************/
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
#pragma once

/// <summary>
/// MAD环,MAD数据储存的重要方式.
/// 注意:该类是线程不安全的!
/// </summary>
/// <typeparam name="T">储存的数据类型</typeparam>
template <class T>
class MADRing {
public:
	struct RingObject {
		T* pT;
		RingObject* pLastT;
		RingObject* pNextT;
		RingObject() {
			pT = nullptr;
			pLastT = nullptr;
			pNextT = nullptr;
		}
		RingObject(T* _pT) {
			pT = _pT;
			pLastT = nullptr;
			pNextT = nullptr;
		}
		RingObject(T* _pT, RingObject* _pLastT) {
			pT = _pT;
			pLastT = _pLastT;
			pNextT = nullptr;
		}
		RingObject(T* _pT, RingObject* _pLastT, RingObject* _pNextT) {
			pT = _pT;
			pLastT = _pLastT;
			pNextT = _pNextT;
		}
	};

public:
	MADRing() {
		Count = 0;
		pFirstObject = nullptr;
		pFocusObject = nullptr;
	}
	~MADRing() {
		Clear();
	}

	/// <summary>
	/// 在聚焦元素的上一个位置串上一个新元素
	/// </summary>
	/// <param name="pAddtion">要添加的元素</param>
	void Append(T* pAddtion) {
		if (pFirstObject == nullptr) {
			pFirstObject = new RingObject(pAddtion);
			pFirstObject->pLastT = pFirstObject;
			pFirstObject->pNextT = pFirstObject;
			pFocusObject = pFirstObject;
			Count++;
			return;
		}
		pFocusObject->pLastT->pNextT = new RingObject(pAddtion, pFocusObject->pLastT, pFocusObject);
		pFocusObject->pLastT = pFocusObject->pLastT->pNextT;
		Count++;
	}

	/// <summary>
	/// 擦除聚焦的元素,这会直接delete元素指针指向的内存(无需手动释放),
	/// 同时聚焦元素会跳转到下一个,如果删除第一个元素,则首元素会变为聚焦元素(即下一个元素)
	/// </summary>
	void Erase() {
		if (pFocusObject == nullptr) {
			return;
		}
		if (Count == 1) {
			Count--;
			delete (pFocusObject->pT);
			delete (pFocusObject);
			pFirstObject = nullptr;
			pFocusObject = nullptr;
			return;
		}
		pFocusObject->pLastT->pNextT = pFocusObject->pNextT;
		pFocusObject->pNextT->pLastT = pFocusObject->pLastT;
		RingObject* buffer = pFocusObject->pNextT;
		if (pFirstObject == pFocusObject) {
			pFirstObject = buffer;
		}
		delete (pFocusObject->pT);
		delete (pFocusObject);
		pFocusObject = buffer;
		Count--;
	}

	/// <summary>
	/// 获取元素数量
	/// </summary>
	/// <returns></returns>
	size_t GetNum() {
		return Count;
	}

	/// <summary>
	/// 清空元素
	/// </summary>
	void Clear() {
		for (size_t i = 0; i < Count; i++) {
			Erase();
		}
		Count = 0;
	}

	/// <summary>
	/// 查看环是否为空
	/// </summary>
	/// <returns>环是否为空</returns>
	bool Is_Empty() {
		return Count == 0;
	}

	/// <summary>
	/// 将焦点后移一位
	/// </summary>
	void MoveToNext() {
		if (pFocusObject == nullptr) {
			return;
		}
		pFocusObject = pFocusObject->pNextT;
	}

	/// <summary>
	/// 获取聚焦的数据指针
	/// </summary>
	/// <returns>目标数据指针</returns>
	T* GetDataPtr() {
		if (pFocusObject == nullptr) {
			return nullptr;
		}
		return pFocusObject->pT;
	}

private:
	RingObject* pFirstObject;
	RingObject* pFocusObject;

	size_t Count;
};

