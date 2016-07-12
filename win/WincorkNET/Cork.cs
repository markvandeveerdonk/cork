using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WincorkNET
{
    public class Cork
    {
        /* Management functions */

        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern void ClearAllMeshes();

        // Deletes mesh with given ID from memory
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool DeleteMesh(int ID);

        // Creates a copy of mesh identified by srcID and store under destID
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool CopyMesh(int srcID, int destID);

        // Loads the mesh from the file specified by fileName and stores it internally under ID
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool LoadMesh(int ID, string fileName);

        // Save the mesh stored internally under ID as file fileName.
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool SaveMesh(int ID, string fileName);

        /* Cork native functions */

        // Can cork handle the model?
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool IsSolid(int ID);

        // Create union model from models stored internally and identified by InID1 and InID2 
        // and store as OutID.
        [DllImport("Wincork.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Union(int ID1, int ID2, int OutID);

        // Create difference model from model stored internally and identified by InID1 and InID2 
        // and store as OutID. Model ID2 is subtraced from ID1.
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool Difference(int ID1, int ID2, int OutID);

        // Create intersection model from models stored internally and identified by InID1 and InID2 
        // and store as OutID.
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool Intersection(int ID1, int ID2, int OutID);

        // Perform XOR operation on models stored internally and identified by InID1 and InID2 
        // and store as OutID.
        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool Xor(int ID1, int ID2, int OutID);

        /* Added manipulation functions */

        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool TranslateZ(int ID, float deltaZ);

        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool Rotate180X(int ID);

        [DllImport("Wincork.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool Rotate180Y(int ID);
    }
}
