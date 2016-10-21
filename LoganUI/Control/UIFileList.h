#ifndef __UIFILELIST_H__
#define __UIFILELIST_H__
#pragma once;

#include <vector>
#include <math.h>
#include <tchar.h>
#include <atlconv.h>

namespace DuiLib 
{
	inline double CalculateDelay(double state)
	{
		return pow(state, 2);
	}
	///////////////////////////////
	///

	class CTreeNodeItemUI : public CListLabelElementUI
	{
	public:
		CTreeNodeItemUI();
	private:
		void DoEvent(TEventUI& event);
	};

	///////////////////////////////
	///

	struct NodeData
	{
		int _level;
		bool _expand;
		bool _select;
		bool loaded;
		CDuiString _expandText;
		CDuiString _selectionBoxText;
		CDuiString _text;
		CDuiString _id;
		CTreeNodeItemUI* _pListElement;
	};

	class UILIB_API Node
	{
	public:
		Node();
		explicit Node(NodeData t);
		Node(NodeData t, Node* parent);
		~Node();

		NodeData& data() ;
		int num_children() const;
		Node* child(int i);
		Node* parent();
		bool has_children() const;

		void add_child(Node* child) ;
		void remove_child(Node* child);
		Node* get_last_child();

	private:
		void set_parent(Node* parent);

	public:
		typedef std::vector <Node*>	Children;
		Children	_children;
		Node*		_parent;
		NodeData    _data;
		bool  LeafFlag;
	};	

	///////////////////////////////
	///
	class UILIB_API CFileListUI : public CListUI
	{
	public:
		CFileListUI();
		~CFileListUI();
		LPCTSTR GetClass() const;

		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();

		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);

		Node* GetRoot();
		Node* GetNode(LPCTSTR text);
		Node* GetNodeByID(LPCTSTR id,Node* pParent=NULL);
		Node* AddNode(LPCTSTR text,Node* parent = NULL, bool leafFlag = false ,LPCTSTR id = _T(""));
		bool RemoveNode(Node* node);	
		void ExpandNode( Node* node );
		void SelectionBoxNode( Node* node, bool select );
		void SelectionBoxNodeCore( Node* node, bool select );
		SIZE GetExpanderSize(Node* node) const;
		SIZE GetSelectionBoxSize( Node* node ) const;

		void SetExpandImg(LPCTSTR sImg);
		void SetNoExpandImg(LPCTSTR sImg);
		void SetSelectImg(LPCTSTR sImg);
		void SetNoSelectImg(LPCTSTR sImg);
		void SetCanSelFlag(bool bCan=true);

	private:
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	private:
		Node *	   _root;
		int		   m_iItemHeight;
		bool	   m_bCanSel;
		CDuiString m_sExpandImg;
		CDuiString m_sNoExpandImg;
		CDuiString m_sSelectImg;
		CDuiString m_sNoSelectImg;
	};

}

#endif
