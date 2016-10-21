#include "StdAfx.h"
#include "UIFileList.h"


namespace DuiLib 
{
	/////////////////////////////////////////////////////////////////////////////////////
	///
	CTreeNodeItemUI::CTreeNodeItemUI()
	{

	}

	void CTreeNodeItemUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_BUTTONDOWN)
		{
			Node* node = (Node*)GetTag();
			POINT pt = event.ptMouse;
			pt.x -= GetX();
			CFileListUI* pList = (CFileListUI*)(GetParent()->GetParent());
			if(pList)
			{
				SIZE sz4Expander = pList->GetExpanderSize(node);
				if( pt.x >= sz4Expander.cx && pt.x < sz4Expander.cy ) 
				{
					pList->ExpandNode(node);
					m_pManager->SendNotify(this,_T("tree_node_expend"),node->data()._level,node->data()._expand);
					return;
				}
				else
				{
					SIZE sz4SelectionBox = pList->GetSelectionBoxSize(node);
					if( pt.x >= sz4SelectionBox.cx && pt.x < sz4SelectionBox.cy )  
					{
						pList->SelectionBoxNode(node,  !node->data()._select );
						m_pManager->SendNotify(this,_T("tree_node_select"),node->data()._level,node->data()._select);
						return;
					}
				}
			}
	
			m_pManager->SendNotify(this,_T("tree_node_click"));
		}
		else if ( event.Type == UIEVENT_DBLCLICK)
		{
			m_pManager->SendNotify(this,_T("tree_node_dclick"));
		}
		__super::DoEvent(event);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	///////////////////
	Node::Node() : _parent (NULL), LeafFlag( false )
	{
		
	}
	Node::Node(NodeData t) : _data (t), _parent (NULL)
	{

	}

	Node::Node(NodeData t, Node* parent)	: _data (t), _parent (parent) 
	{

	}

	Node::~Node() 
	{
		for (int i = 0; i < num_children(); i++)
		{
			delete _children[i];  
		}
	}

	NodeData& Node::data() 
	{ 
		return _data; 
	}	

	int Node::num_children() const
	{ 
		return _children.size();
	}

	Node* Node::child(int i)	
	{ 
		return _children.at(i); 
	}

	Node* Node::parent() 
	{ 
		return ( _parent);	
	}

	bool Node::has_children() const 
	{	
		return num_children() > 0;
	}

	void Node::add_child(Node* child) 
	{
		child->set_parent(this); 
		_children.push_back(child); 
	}

	void Node::remove_child(Node* child)
	{
		Children::iterator iter = _children.begin();
		for( ; iter < _children.end(); ++iter )
		{
			if( *iter == child ) 
			{
				_children.erase(iter);
				return;
			}
		}
	}

	Node* Node::get_last_child()
	{
		if( has_children() )
		{
			return child(num_children() - 1)->get_last_child();
		}
		else return this;
	}

	void Node::set_parent(Node* parent) 
	{ 
		_parent = parent; 
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CFileListUI::CFileListUI() : _root(NULL),
		m_iItemHeight(26),
		m_bCanSel(false)
	{
		SetItemShowHtml(true);

		_root = new Node;
		_root->data()._level = -1;
		_root->data()._expand = true;
		_root->data()._select = false;
		_root->data()._text = _T("");
		_root->data()._id = _T("");
		_root->data()._pListElement = NULL;
	}

	CFileListUI::~CFileListUI() 
	{ 
		if(_root) 
			delete _root; 
	}

	LPCTSTR CFileListUI::GetClass() const
	{
		return _T("FileList");
	}

	bool CFileListUI::Add(CControlUI* pControl)
	{
		if( !pControl ) 
			return false;
		if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) != 0 ) 
			return false;
		return CListUI::Add(pControl);
	}

	bool CFileListUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if( !pControl ) 
			return false;
		if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) != 0 ) 
			return false;
		return CListUI::AddAt(pControl, iIndex);
	}

	bool CFileListUI::Remove(CControlUI* pControl)
	{
		if( !pControl ) 
			return false;
		if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) != 0 ) 
			return false;

		if (reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(_T("ListLabelElement")))->GetTag()) == NULL)
			return CListUI::Remove(pControl);
		else
			return RemoveNode(reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(_T("ListLabelElement")))->GetTag()));
	}

	bool CFileListUI::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if( !pControl ) 
			return false;
		if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) != 0 ) 
			return false;

		if (reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(_T("ListLabelElement")))->GetTag()) == NULL)
			return CListUI::RemoveAt(iIndex);
		else
			return RemoveNode(reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(_T("ListLabelElement")))->GetTag()));
	}

	void CFileListUI::RemoveAll()
	{
		CListUI::RemoveAll();
		for (int i = 0; i < _root->num_children(); ++i)
		{
			Node* child = _root->child(i);
			RemoveNode(child);
		}
		delete _root;
		_root = new Node;
		_root->data()._level = -1;
		_root->data()._expand = true;
		_root->data()._pListElement = NULL;
	}

	void CFileListUI::SetVisible(bool bVisible )
	{
		if( m_bVisible == bVisible ) return;
		CControlUI::SetVisible(bVisible);
	}

	void CFileListUI::SetInternVisible(bool bVisible )
	{
		CControlUI::SetInternVisible(bVisible);
	}
	
	Node* CFileListUI::GetRoot() 
	{ 
		return _root; 
	}

	Node* CFileListUI::GetNode(LPCTSTR text)
	{
		std::vector <Node*>::iterator ite = _root->_children.begin();
		Node* pRt = _root;
		while(ite != _root->_children.end())
		{
			if(0 == (*ite)->_data._text.Compare(text))
			{
				pRt =  *ite;
				break;
			}
			++ite;
		}
		return pRt;
	}

	Node* CFileListUI::GetNodeByID(LPCTSTR id,Node* pParent)
	{
		Node* pRt = NULL;
		if(!pParent)
			pParent = _root;

		std::vector <Node*>::iterator ite = pParent->_children.begin();
		while(ite != pParent->_children.end())
		{
			if(0 == (*ite)->_data._id.Compare(id))
			{
				pRt =  *ite;
				break;
			}
			else
				pRt = GetNodeByID(id,*ite);

			++ite;
		}
		return pRt;
	}

	bool CFileListUI::RemoveNode(Node* node)
	{
		if( !node || node == _root ) 
			return false;

		for( int i = 0; i < node->num_children(); ++i ) 
		{
			Node* child = node->child(i);
			RemoveNode(child);
		}
		CListUI::Remove(node->data()._pListElement);
		node->parent()->remove_child(node);
		delete node;
		return true;
	}

	Node* CFileListUI::AddNode(LPCTSTR text,Node* parent, bool leafFlag,LPCTSTR id)
	{
		CDuiString html_text; 
		if( !parent ) 
			parent = _root;

		CTreeNodeItemUI* pListElement = new CTreeNodeItemUI;
		pListElement->SetFixedWidth(m_rcItem.right-m_rcItem.left-2);
		pListElement->SetFixedHeight(m_iItemHeight);
		Node* node = new Node;
		node->data()._level = parent->data()._level + 1;
		node->data()._expand = false;
		node->data()._select = false;
		node->data()._text = text;
		node->data()._id = id;
		node->data()._pListElement = pListElement;
		node->LeafFlag = leafFlag;

		if( parent != _root ) 
		{
			if( !(parent->data()._expand && parent->data()._pListElement->IsVisible()) )
				pListElement->SetInternVisible(false);
		}

		html_text += _T("<x 6>");
		for( int i = 0; i < node->data()._level; ++i )
		{
			html_text += _T("<x 24>"); 
		}
		
		if( !node->LeafFlag )
		{
			html_text += m_sExpandImg;
			if(m_bCanSel)
				node->data()._selectionBoxText.Format(_T("<x 8> %s <x 2>"),m_sNoSelectImg.GetData());
		}
		else
		{
			if(m_bCanSel)
				node->data()._selectionBoxText.Format(_T("<x 16> %s <x 2>"),m_sNoSelectImg.GetData());
		}

		node->data()._expandText = html_text;
		
		CDuiString sTemp = node->data()._expandText;
		if(m_bCanSel)
			sTemp += node->data()._selectionBoxText;
		else 
			sTemp += _T("<x 6>");
		sTemp += node->data()._text;
		pListElement->SetText(sTemp);

		pListElement->SetTag((UINT_PTR)node);

		int index = 0;
		if( parent->has_children() ) 
		{
			Node* prev = parent->get_last_child();
			index = prev->data()._pListElement->GetIndex() + 1;
		}
		else
		{
			if( parent == _root ) 
				index = 0;
			else 
				index = parent->data()._pListElement->GetIndex() + 1;
		}

		if( !CListUI::AddAt(pListElement, index) )
		{
			delete pListElement;
			delete node;
			node = NULL;
		}

		parent->add_child(node);
		return node;
	}

	void CFileListUI::ExpandNode(Node* node)
	{
		bool expand = !node->data()._expand;
		if( !node || node == _root )
			return;

		if( node->data()._expand == expand )
			return;

		node->data()._expand = expand;

		CDuiString html_text;
		if(node->_data._level == 0)
		{
			
		}

		html_text += _T("<x 6>");
		for( int i = 0; i < node->data()._level; ++i ) 
		{
			html_text += _T("<x 24>");
		}
		if( !node->LeafFlag )
		{
			if( node->data()._expand ) 
				html_text += m_sNoExpandImg;
			else 
				html_text += m_sExpandImg;
		}
		if( !node->LeafFlag )
		{
			if( node->data()._expand ) 
				html_text += _T("<a><i notexpand.png></a>");
			else 
				html_text += _T("<a><i expand.png></a>");
		}
		node->data()._expandText = html_text;

		CDuiString sTemp =  node->data()._expandText;
		if(m_bCanSel)
			sTemp += node->data()._selectionBoxText;
		else 
			sTemp += _T("<x 6>");
		sTemp += node->data()._text;
		node->data()._pListElement->SetText(sTemp);

		if( !node->data()._pListElement->IsVisible() ) 
			return;
		if( !node->has_children() ) 
			return;

		Node* begin = node->child(0);
		Node* end = node->get_last_child();
		for( int i = begin->data()._pListElement->GetIndex(); i <= end->data()._pListElement->GetIndex(); ++i ) 
		{
			CControlUI* control = GetItemAt(i);
			if( _tcscmp(control->GetClass(), _T("ListLabelElementUI")) == 0 )
			{
				Node* local_parent = ((Node*)control->GetTag())->parent();
				control->SetInternVisible(local_parent->data()._expand && local_parent->data()._pListElement->IsVisible());
			}
		}
		NeedUpdate();
	}

	void CFileListUI::SelectionBoxNode( Node* node, bool select )
	{
		SelectionBoxNodeCore( node, select );
		Node* begin;
		Node* end;
		if( !node->LeafFlag && node->num_children())
		{
			begin = node->child(0);
			end = node->get_last_child();
			for( int i = begin->data()._pListElement->GetIndex(); i <= end->data()._pListElement->GetIndex(); ++i ) 
			{
				CControlUI* control = GetItemAt(i);
				Node* childNode = (Node*)control->GetTag();
				SelectionBoxNodeCore( childNode, select );
			}	
		}

		Node* parent = NULL;
		parent = node->parent();
		if( !parent )
			return;

		if( select )
		{
			do
			{
				begin = parent->child(0);
				end = parent->get_last_child();
				int index;
				for( index = begin->data()._pListElement->GetIndex(); index <= end->data()._pListElement->GetIndex(); ++index ) 
				{
					CControlUI* control = GetItemAt(index);
					Node* childNode = (Node*)control->GetTag();
					if( childNode->data()._select != select )
						break;
				}
				if( index >end->data()._pListElement->GetIndex() )
					SelectionBoxNodeCore( parent, select );

				parent = parent->parent();
				if( !parent )
					break;

			}while( true );
		}
		else
		{
			do
			{
				SelectionBoxNodeCore( parent, select );
				parent = parent->parent();
				if( !parent )
					break;
			}while( true );
		}
	}

	void CFileListUI::SelectionBoxNodeCore( Node* node, bool select )
	{
		if( !node || node == _root )
			return;

		if( node->data()._select == select )
			return;

		node->data()._select = select;
			
		if( !node->LeafFlag )
		{	
			//if( node->_parent != _root ) //去除根节点的全选
			{
				if( node->data()._select )
					node->data()._selectionBoxText.Format(_T("<x 8> %s <x 2>"),m_sSelectImg.GetData());
				else
					node->data()._selectionBoxText.Format(_T("<x 8> %s <x 2>"),m_sNoSelectImg.GetData());
			}
		}
		else
		{
			if( node->data()._select )
				node->data()._selectionBoxText.Format(_T("<x 16> %s <x 2>"),m_sSelectImg.GetData());
			else
				node->data()._selectionBoxText.Format(_T("<x 16> %s <x 2>"),m_sNoSelectImg.GetData());
		}

		node->data()._pListElement->SetText( node->data()._expandText + node->data()._selectionBoxText + node->data()._text );

		if( !node->data()._pListElement->IsVisible() ) 
			return;
		if( !node->has_children() ) 
			return;

		NeedUpdate();
	}

	SIZE CFileListUI::GetExpanderSize(Node* node) const
	{
		if( !node || node == _root ) 
			return CSize();

	/*	if( node->LeafFlag )
			return CSize();*/

		SIZE szExpander = {0};
		szExpander.cx += 6 + 24 * node->data()._level;
		szExpander.cy = szExpander.cx + 11;	
		return szExpander;
	}

	SIZE CFileListUI::GetSelectionBoxSize( Node* node ) const
	{
		if( !node || node == _root || !m_bCanSel) 
			return CSize();

		SIZE szSelectionBox = {0};
		if( !node->LeafFlag )	
			szSelectionBox.cx = 6 + 24 * node->data()._level  + 11 + 6;
		else
			szSelectionBox.cx = 6 + 24 * node->data()._level  + 11 + 6;
	
		szSelectionBox.cy = szSelectionBox.cx + 15;

		return szSelectionBox;
	}

	void CFileListUI::SetExpandImg(LPCTSTR sImg)
	{
		m_sExpandImg.Format(_T("<i %s >"),sImg);
	}

	void CFileListUI::SetNoExpandImg(LPCTSTR sImg)
	{
		m_sNoExpandImg.Format(_T("<i %s >"),sImg);
	}

	void CFileListUI::SetSelectImg(LPCTSTR sImg)
	{
		m_sSelectImg.Format(_T("<i %s >"),sImg);
	}

	void CFileListUI::SetNoSelectImg(LPCTSTR sImg)
	{
		m_sNoSelectImg.Format(_T("<i %s >"),sImg);
	}

	void CFileListUI::SetCanSelFlag(bool bCan)
	{
		m_bCanSel = bCan;
	}

	void CFileListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("expandimgage")) == 0 )
			SetExpandImg(pstrValue);
		else if( _tcscmp(pstrName, _T("noexpandimgage")) == 0 )
			SetNoExpandImg(pstrValue);
		else if( _tcscmp(pstrName, _T("selectimgage")) == 0 )
			SetSelectImg(pstrValue);
		else if( _tcscmp(pstrName, _T("noselectimgage")) == 0 )
			SetNoSelectImg(pstrValue);
		if( _tcscmp(pstrName, _T("canselect")) == 0 )
		{
			LPTSTR pstr = NULL;
			int nValue = _tcstol(pstrValue, &pstr, 10);  
			ASSERT(pstr);
			SetCanSelFlag(nValue);
		}
		else
			__super::SetAttribute(pstrName,pstrValue);
	}

}