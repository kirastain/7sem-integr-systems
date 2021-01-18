using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Runtime.InteropServices;

namespace SocketSharp
{
	public enum Messages: uint
	{
		M_INIT,
		M_EXIT,
		M_GETDATA,
		M_NODATA,
		M_DATA,
		M_CONFIRM
	};

	public enum Members: uint
	{
		M_BROKER = 0,
		M_ALL = 10,
		M_USER = 100
	};

	public struct MsgHeader
	{
		public uint m_To;
		public uint m_From;
		public uint m_Type;
		public uint m_Size;
	};


	class Message
	{
		//ппотому что мы ленивые котлетки
		static Encoding cp866 = Encoding.GetEncoding("CP866");

		public MsgHeader m_Header;
		public string m_Data;

		public Message(uint To, uint From, uint Type = (uint)Messages.M_DATA, string Data = "")
		{
			this.m_Header = new MsgHeader { m_To = To, m_From = From, m_Type = Type, m_Size = (uint)Data.Length };
			this.m_Data = Data;
		}

		public void Send(Socket s)
		{
			s.Send(StructToBytes(this.m_Header), Marshal.SizeOf(this.m_Header), SocketFlags.None);

			if (this.m_Header.m_Size != 0)
			{
				s.Send(cp866.GetBytes(this.m_Data), Convert.ToInt32(this.m_Header.m_Size), SocketFlags.None);
			}
		}

		public int Recieve(Socket s)
		{
			//хэдер пошел
			byte[] b = new byte[Marshal.SizeOf(this.m_Header)];
			s.Receive(b, Marshal.SizeOf(this.m_Header), SocketFlags.None);
			//хэдер получен
			this.m_Header = BytesToStruct(b);

			//прочекали и пока живем
			if (this.m_Header.m_Size != 0)
            {
				b = new byte[this.m_Header.m_Size];
				s.Receive(b, Convert.ToInt32(this.m_Header.m_Size), SocketFlags.None);
				this.m_Data = cp866.GetString(b, 0, Convert.ToInt32(this.m_Header.m_Size));
			}
			return Convert.ToInt32(this.m_Header.m_Type);
		}

		//это разбираем на байты хэдер
		static byte[] StructToBytes(MsgHeader myStruct)
		{
			int size = Marshal.SizeOf(myStruct);
			byte[] arr = new byte[size];

			IntPtr buffer = Marshal.AllocHGlobal(size);
			Marshal.StructureToPtr(myStruct, buffer, false);
			Marshal.Copy(buffer, arr, 0, size);
			Marshal.FreeHGlobal(buffer);

			return arr;
		}

		static MsgHeader BytesToStruct(byte[] arr)
		{
			int size = Marshal.SizeOf(typeof(MsgHeader));

			IntPtr buffer = Marshal.AllocHGlobal(size);
			Marshal.Copy(arr, 0, buffer, size);
			MsgHeader myStruct = (MsgHeader)Marshal.PtrToStructure(buffer, typeof(MsgHeader));
			Marshal.FreeHGlobal(buffer);

			return myStruct;
		}
	}
}
