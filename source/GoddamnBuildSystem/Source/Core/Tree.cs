//! ==========================================================================================
//! Extensions.cs - extensions to .NET standart library.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 26.11.2014 - Created by James Jhuighuy
//! ==========================================================================================

using System.IO;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
	/// <summary>
	/// Represents a tree in which each node has a label and a list of elements.
	/// Is internally used in BuildSystem to prepresent filesystem emulation.
	/// </summary>
	public sealed class MultiTree<T>
	{
		//! ------------------------------------------------------------------------------------------
		//! Internal types.
		//! ------------------------------------------------------------------------------------------
 
		/// <summary>
		/// Labeled multi tree node.
		/// </summary>
		public sealed class Node
		{
			//! ------------------------------------------------------------------------------------------
			//! Fields.
			//! ------------------------------------------------------------------------------------------
 
			private string m_Label = null;
			private List<T> m_Elements = null;
			private List<Node> m_ChildNodes = null;
			private Node m_RootNode = null;
 
			//! ------------------------------------------------------------------------------------------
			//! Constructors.
			//! ------------------------------------------------------------------------------------------
 
			public Node(Node Root, string Label) 
			{
				m_RootNode = Root;
				m_Label = Label;
				Root.GetChildNodes().Add(this);
			}
 
			public Node(MultiTree<T> Tree)
			{
				m_RootNode = null;
				m_Label = null;
 
				Tree.GetChildNodes().Add(this);
			}
 
			//! ------------------------------------------------------------------------------------------
			//! Getters.
			//! ------------------------------------------------------------------------------------------
 
			/// <summary>
			/// Returns the label.
			/// </summary>
			public string GetLabel()
			{
				return m_Label;
			}
 
			/// <summary>
			/// List of the node elements.
			/// </summary>
			public List<T> GetElements()
			{
				return m_Elements;
			}
 
			/// <summary>
			/// Returns the root node.
			/// </summary>
			public Node GetRootNode()
			{
				return m_RootNode;
			}
 
			/// <summary>
			/// Returns the list of child nodes.
			/// </summary>
			public List<Node> GetChildNodes()
			{
				if (m_ChildNodes == null) {
					m_ChildNodes = new List<Node> ();
				}
 
				return m_ChildNodes;
			}
 
			//! ------------------------------------------------------------------------------------------
			//! Class API.
			//! ------------------------------------------------------------------------------------------
 
			/// <summary>
			/// Returns the path to node from root.
			/// </summary>
			public string GetNodePath()
			{
				string NodePath = "";
				for (Node TheNode = this; TheNode != null; TheNode = TheNode.m_RootNode) {
					NodePath = string.Concat(TheNode.m_Label, Path.DirectorySeparatorChar, NodePath);
				}
 
				return NodePath;
			}
 
			/// <summary>
			/// Creates the child node with specified label.
			/// </summary>
			public Node CreateChildNode(string Label)
			{
				return new Node(this, Label);
			}
		}	// class LabeledMultiTreeNode
 
		//! ------------------------------------------------------------------------------------------
		//! Fields.
		//! ------------------------------------------------------------------------------------------

        private Node m_RootNode = null;
 
		//! ------------------------------------------------------------------------------------------
		//! Getters.
		//! ------------------------------------------------------------------------------------------
 
		/// <summary>
        /// Returns the root node of this tree.
		/// </summary>
		public Node GetRootNode()
		{
			if (m_RootNode == null) {
                m_RootNode = new Node();
			}

            return m_RootNode;
		}
	}	// class LabeledMutliTree
}
