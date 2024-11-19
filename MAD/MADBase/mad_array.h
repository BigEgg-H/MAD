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
/// MAD��,MAD���ݴ������Ҫ��ʽ.
/// ע��:�������̲߳���ȫ��!
/// </summary>
/// <typeparam name="T">�������������</typeparam>
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
	/// �ھ۽�Ԫ�ص���һ��λ�ô���һ����Ԫ��
	/// </summary>
	/// <param name="pAddtion">Ҫ��ӵ�Ԫ��</param>
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
	/// �����۽���Ԫ��,���ֱ��deleteԪ��ָ��ָ����ڴ�(�����ֶ��ͷ�),
	/// ͬʱ�۽�Ԫ�ػ���ת����һ��,���ɾ����һ��Ԫ��,����Ԫ�ػ��Ϊ�۽�Ԫ��(����һ��Ԫ��)
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
	/// ��ȡԪ������
	/// </summary>
	/// <returns></returns>
	size_t GetNum() {
		return Count;
	}

	/// <summary>
	/// ���Ԫ��
	/// </summary>
	void Clear() {
		for (size_t i = 0; i < Count; i++) {
			Erase();
		}
		Count = 0;
	}

	/// <summary>
	/// �鿴���Ƿ�Ϊ��
	/// </summary>
	/// <returns>���Ƿ�Ϊ��</returns>
	bool Is_Empty() {
		return Count == 0;
	}

	/// <summary>
	/// ���������һλ
	/// </summary>
	void MoveToNext() {
		if (pFocusObject == nullptr) {
			return;
		}
		pFocusObject = pFocusObject->pNextT;
	}

	/// <summary>
	/// ��ȡ�۽�������ָ��
	/// </summary>
	/// <returns>Ŀ������ָ��</returns>
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

