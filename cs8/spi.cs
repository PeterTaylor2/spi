using System;
using System.Runtime.InteropServices;

namespace SPI
{
    public class spi
    {
        /* Error handling functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern string spi_Error_get();

        public delegate void IntPtr_delete(IntPtr v);

        // The point of PointerHandle is to contain a converted value which is
        // represented as IntPtr and to manage the memory associated with the IntPtr
        //
        // Inputs to a function in C# are converted and may well create IntPtr
        // We put the IntPtr inside a PointerHandle and then pass get_inner() to
        // the C function
        //
        // Outputs of type IntPtr from a function in C should be immediately wrapped
        // inside PointerHandle before the C to C# conversion is performed.
        // This makes it easy to dispose of the IntPtr on exiting the function.

        public class PointerHandle : IDisposable
        {
            public PointerHandle(IntPtr in_ptr, IntPtr_delete in_deleter) 
            {
                ptr = in_ptr;
                deleter = in_deleter;
            }

            public IntPtr get_inner()
            {
                return ptr;
            }

            private IntPtr ptr;
            private IntPtr_delete deleter;

            ~PointerHandle()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            protected void Dispose(bool dispose)
            {
                if (ptr != IntPtr.Zero)
                {
                    deleter(ptr);
                    ptr = IntPtr.Zero;
                }
            }
        }

        public static Exception ErrorToException()
        {
            System.String error = spi_Error_get();
            return new Exception(error);
        }

        /* Date functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_from_YMD(
            int year, int month, int day, out int date);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_YMD(
            int date, out int year, out int month, out int day);

        public static System.DateTime DateFromCDate(int dt)
        {
            if (dt == 0)
                return default(System.DateTime);

            const long oneDay = 86400;
            const long oneE7 = 10000000;
            const long offset = 584388;
            long ticks = ((long)dt + offset) * oneDay * oneE7;
            return new System.DateTime(ticks);
        }

        public static int DateToCDate(System.DateTime dt)
        { 
            if (dt == default(System.DateTime))
                return 0;

            const long oneDay = 86400;
            const long oneE7 = 10000000;
            const long offset = 584388;
            long cdt = dt.Ticks / (oneDay * oneE7) - offset;
            return (int)cdt;
        }

        /* DateTime functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_from_YMDHMS(
            int year, int month, int day, int hours, int minutes, int seconds,
            out double dateTime);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_YMDHMS(
            double date,
            out int year, out int month, out int day,
            out int hours, out int minutes, out int seconds);

        public static System.DateTime DateTimeFromCDateTime(double dt)
        {
            if (dt == 0.0)
                return default(System.DateTime);

            const long oneDay = 86400;
            const long oneE7 = 10000000;
            const long offset = 584388;
            long ticks = (long)((dt + offset) * oneDay * oneE7 + 0.5);
            return new System.DateTime(ticks);
        }

        public static double DateTimeToCDateTime(System.DateTime dt)
        {
            if (dt == default(System.DateTime))
                return 0.0;

            const long oneDay = 86400;
            const long oneE7 = 10000000;
            const long offset = 584388;
            double cdt = (double)(dt.Ticks) / (oneDay * oneE7) - offset;
            return cdt;
        }

        ///* Object functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Object_delete(IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_get_object_id(IntPtr item, out string objectId);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_get_class_name(IntPtr item, out string className);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_to_string(
            IntPtr self,
            string format,
            string options,
            out string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_from_string(
            string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_to_file(
            IntPtr self,
            string filename,
            string format,
            string options);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_from_file(
            string filename);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_get_value(
            IntPtr self,
            string name);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_save(
            string baseName,
            IntPtr obj,
            bool noCount,
            out string handle);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_find(
            string handle,
            out IntPtr obj);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_free_all(
            out int count);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_count(
            string className,
            out int count);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_free(
            string handle);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_list(
            string baseName,
            string className,
            out IntPtr handles);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_class_name(
            string handle,
            out string className);

        /* Variant functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Variant_delete(IntPtr self);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_String(string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Date(int dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_DateTime(double dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Double(double d);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Bool(bool b);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Int(int i);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_String(
            IntPtr var,
            out string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Date(
            IntPtr var,
            out int dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_DateTime(
            IntPtr var,
            out double dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Double(
            IntPtr var,
            out double d);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Bool(
            IntPtr var,
            out bool b);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Int(
            IntPtr var,
            out int i);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Object(
            IntPtr var,
            out IntPtr obj);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_String_Vector(
            IntPtr var,
            out IntPtr str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Date_Vector(
            IntPtr var,
            out IntPtr dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_DateTime_Vector(
            IntPtr var,
            out IntPtr dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Double_Vector(
            IntPtr var,
            out IntPtr d);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Bool_Vector(
            IntPtr var,
            out IntPtr b);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Int_Vector(
            IntPtr var,
            out IntPtr i);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Object_Vector(
            IntPtr var,
            out IntPtr obj);

        /* vector functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Int_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Int_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Vector_get_data(
            IntPtr v,
            int N,
            int[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Vector_set_data(
            IntPtr v,
            int N,
            int[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle IntVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Int_Vector_delete);
        }

        public static int[] IntVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Int_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            int[] array = new int[size];

            if (size > 0)
            {
                if (spi_Int_Vector_get_data(v, size, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle IntVectorFromArray(int[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Int_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Int_Vector_delete);
            if (array is not null)
            {
                if (spi_Int_Vector_set_data(v, size, array) != 0)
                    throw ErrorToException();
            }
            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Double_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Double_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Vector_get_data(
            IntPtr v,
            int N,
            double[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Vector_set_data(
            IntPtr v,
            int N,
            double[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle DoubleVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Double_Vector_delete);
        }

        public static double[] DoubleVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Double_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            double[] array = new double[size];

            if (size > 0)
            {
                if (spi_Double_Vector_get_data(v, size, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle DoubleVectorFromArray(double[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Double_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Double_Vector_delete);
            if (array is not null)
            {
                if (spi_Double_Vector_set_data(v, size, array) != 0)
                    throw ErrorToException();
            }
            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Bool_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Bool_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Vector_get_data(
            IntPtr v,
            int N,
            bool[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Vector_set_data(
            IntPtr v,
            int N,
            bool[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle BoolVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Bool_Vector_delete);
        }

        public static bool[] BoolVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Bool_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            bool[] array = new bool[size];

            if (size > 0)
            {
                if (spi_Bool_Vector_get_data(v, size, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle BoolVectorFromArray(bool[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Bool_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Bool_Vector_delete);
            if (array is not null)
            {
                if (spi_Bool_Vector_set_data(v, size, array) != 0)
                    throw ErrorToException();
            }
            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Date_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Date_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Vector_get_data(
            IntPtr v,
            int N,
            int[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Vector_set_data(
            IntPtr v,
            int N,
            int[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle DateVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Date_Vector_delete);
        }

        public static System.DateTime[] DateVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Date_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[] array = new System.DateTime[size];
            int[] data = new int[size];
            if (spi_Date_Vector_get_data(v, size, data) != 0)
            {
                throw ErrorToException();
            }
            for (int i = 0; i < size; ++i)
            {
                array[i] = DateFromCDate(data[i]);
            }
            return array;
        }

        public static PointerHandle DateVectorFromArray(System.DateTime[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Date_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }

            PointerHandle h = new PointerHandle(v, spi_Date_Vector_delete);

            if (array is not null && size > 0)
            {
                int[] data = new int[size];
                for (int i = 0; i < size; ++i)
                {
                    data[i] = DateToCDate(array[i]);
                }
                if (spi_Date_Vector_set_data(v, size, data) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_DateTime_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_DateTime_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Vector_get_data(
            IntPtr v,
            int N,
            double[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Vector_set_data(
            IntPtr v,
            int N,
            double[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Vector_size(
            IntPtr v,
            out int size);


        public static PointerHandle DateTimeVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_DateTime_Vector_delete);
        }

        public static System.DateTime[] DateTimeVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_DateTime_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[] array = new System.DateTime[size];
            double[] data = new double[size];
            if (spi_DateTime_Vector_get_data(v, size, data) != 0)
            {
                throw ErrorToException();
            }
            for (int i = 0; i < size; ++i)
            {
                array[i] = DateTimeFromCDateTime(data[i]);
            }
            return array;
        }

        public static PointerHandle DateTimeVectorFromArray(System.DateTime[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_DateTime_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }

            PointerHandle h = new PointerHandle(v, spi_DateTime_Vector_delete);

            if (array is not null && size > 0)
            {
                double[] data = new double[size];
                for (int i = 0; i < size; ++i)
                {
                    data[i] = DateTimeToCDateTime(array[i]);
                }
                if (spi_DateTime_Vector_set_data(v, size, data) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_String_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_String_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Vector_item(
            IntPtr v,
            int i,
            out string item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Vector_set_data(
            IntPtr v,
            int N,
            string[] item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle StringVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_String_Vector_delete);
        }

        public static string[] StringVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_String_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            string[] array = new string[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_String_Vector_item(v, i, out array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle StringVectorFromArray(string[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_String_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }

            PointerHandle h = new PointerHandle(v, spi_String_Vector_delete);

            if (array is not null && size > 0)
            {
                if (spi_String_Vector_set_data(v, size, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Object_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Vector_item(
            IntPtr v,
            int i,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Vector_set_item(
            IntPtr v,
            int i,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Vector_size(
            IntPtr v,
            out int size);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Vector_get_data(
            IntPtr v,
            int N,
            IntPtr[] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Vector_size(
            IntPtr v,
            out int size);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Variant_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Vector_item(
            IntPtr v,
            int i,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Vector_set_item(
            IntPtr v,
            int i,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Vector_size(
            IntPtr v,
            out int size);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Map_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Map_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Vector_item(
            IntPtr v,
            int i,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Vector_set_item(
            IntPtr v,
            int i,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Vector_size(
            IntPtr v,
            out int size);

        /* matrix functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Int_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Int_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Matrix_get_data(
            IntPtr m,
            int nr, int nc,
            int[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Matrix_set_data(
            IntPtr m,
            int nr, int nc,
            int[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle IntMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Int_Matrix_delete);
        }

        public static int[,] IntMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Int_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            int[,] array = new int[nr, nc];

            if (spi_Int_Matrix_get_data(m, nr, nc, array) != 0)
            {
                throw ErrorToException();
            }
            return array;
        }

        public static PointerHandle IntMatrixFromArray(int[,]? array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Int_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Int_Matrix_delete);

            if (array is not null)
            {
                if (spi_Int_Matrix_set_data(m, nr, nc, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Double_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Double_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Matrix_get_data(
            IntPtr m,
            int nr, int nc,
            double[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Matrix_set_data(
            IntPtr m,
            int nr, int nc,
            double[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle DoubleMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Double_Matrix_delete);
        }

        public static double[,] DoubleMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Double_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            double[,] array = new double[nr, nc];

            if (spi_Double_Matrix_get_data(m, nr, nc, array) != 0)
            {
                throw ErrorToException();
            }
            return array;
        }

        public static PointerHandle DoubleMatrixFromArray(double[,]? array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Double_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Double_Matrix_delete);

            if (array is not null)
            {
                if (spi_Double_Matrix_set_data(m, nr, nc, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Bool_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Bool_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Matrix_get_data(
            IntPtr m,
            int nr, int nc,
            bool[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Matrix_set_data(
            IntPtr m,
            int nr, int nc,
            bool[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle BoolMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Bool_Matrix_delete);
        }

        public static bool[,] BoolMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Bool_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            bool[,] array = new bool[nr, nc];

            if (spi_Bool_Matrix_get_data(m, nr, nc, array) != 0)
            {
                throw ErrorToException();
            }

            return array;
        }

        public static PointerHandle BoolMatrixFromArray(bool[,]? array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Bool_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Bool_Matrix_delete);

            if (array is not null)
            {
                if (spi_Bool_Matrix_set_data(m, nr, nc, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Date_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Date_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Matrix_get_data(
            IntPtr m,
            int nr, int nc,
            int[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Matrix_set_data(
            IntPtr m,
            int nr, int nc,
            int[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);


        public static PointerHandle DateMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Date_Matrix_delete);
        }

        public static System.DateTime[,] DateMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Date_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[,] array = new System.DateTime[nr, nc];
            if (nr > 0 && nc > 0)
            {
                int[,] data = new int[nr, nc];
                if (spi_Date_Matrix_get_data(m, nr, nc, data) != 0)
                {
                    throw ErrorToException();
                }
                for (int i = 0; i < nr; ++i)
                {
                    for (int j = 0; j < nc; ++j)
                    {
                        array[i, j] = spi.DateFromCDate(data[i, j]);
                    }
                }
            }
            return array;
        }

        public static PointerHandle DateMatrixFromArray(System.DateTime[,]?array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Date_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Date_Matrix_delete);

            if (array is not null)
            {
                int[,] data = new int[nr, nc];
                for (int i = 0; i < nr; ++i)
                {
                    for (int j = 0; j < nc; ++j)
                    {
                        data[i, j] = spi.DateToCDate(array[i, j]);
                    }
                }
                if (spi_Date_Matrix_set_data(m, nr, nc, data) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_DateTime_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_DateTime_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Matrix_get_data(
            IntPtr m,
            int nr, int nc,
            double[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Matrix_set_data(
            IntPtr m,
            int nr, int nc,
            double[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle DateTimeMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_DateTime_Matrix_delete);
        }

        public static System.DateTime[,] DateTimeMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_DateTime_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[,] array = new System.DateTime[nr, nc];
            if (nr > 0 && nc > 0)
            {
                double[,] data = new double[nr, nc];
                if (spi_DateTime_Matrix_get_data(m, nr, nc, data) != 0)
                {
                    throw ErrorToException();
                }
                for (int i = 0; i < nr; ++i)
                {
                    for (int j = 0; j < nc; ++j)
                    {
                        array[i, j] = spi.DateTimeFromCDateTime(data[i, j]);
                    }
                }
            }
            return array;
        }

        public static PointerHandle DateTimeMatrixFromArray(System.DateTime[,]? array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_DateTime_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_DateTime_Matrix_delete);

            if (array is not null)
            {
                double[,] data = new double[nr, nc];
                for (int i = 0; i < nr; ++i)
                {
                    for (int j = 0; j < nc; ++j)
                    {
                        data[i, j] = spi.DateTimeToCDateTime(array[i, j]);
                    }
                }
                if (spi_DateTime_Matrix_set_data(m, nr, nc, data) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_String_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_String_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Matrix_item(
            IntPtr m,
            int r, int c,
            out string item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Matrix_set_data(
            IntPtr m,
            int nr, int nc,
            string[,] data);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle StringMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_String_Matrix_delete);
        }

        public static string[,] StringMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_String_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            string[,] array = new string[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_String_Matrix_item(m, i, j, out array[i, j]) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }
            return array;
        }

        public static PointerHandle StringMatrixFromArray(string[,]? array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_String_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_String_Matrix_delete);

            if (array is not null)
            {
                if (spi_String_Matrix_set_data(m, nr, nc, array) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Object_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Matrix_item(
            IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Matrix_item(
             IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);


        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Matrix_item(
             IntPtr m,
            int r, int c,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Matrix_set_item(
            IntPtr m,
            int r, int c,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);


        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_type(IntPtr m, out string vt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_array_element_type(IntPtr m, out string vt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_array_num_dimensions(IntPtr m, out int ND);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_array_dimensions(IntPtr m, int ND, int[] dimensions);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Variant_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Matrix_item(
            IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Map_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Map_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Matrix_item(
            IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public class SpiVariant : IDisposable
        {
            public SpiVariant(string str)
            {
                self = spi_Variant_new_String(str);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public SpiVariant(System.DateTime dt)
            {
                self = spi_Variant_new_DateTime(DateTimeToCDateTime(dt));
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public SpiVariant(double d)
            {
                self = spi_Variant_new_Double(d);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public SpiVariant(bool b)
            {
                self = spi_Variant_new_Bool(b);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public SpiVariant(int i)
            {
                self = spi_Variant_new_Int(i);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            ~SpiVariant()
            {
                Dispose(false);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (self != IntPtr.Zero)
                {
                    spi_Variant_delete(self);
                    self = IntPtr.Zero;
                }
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            public SpiVariant(IntPtr in_self)
            {
                self = in_self;
            }

            public static IntPtr get_inner(SpiVariant v)
            {
                if (v == null)
                    return IntPtr.Zero;

                return v.self;
            }

            /// returns the type of the Variant with the following possible values:
            /// Undefined, Char, String, Int, Double, Bool, Date, DateTime, Map, Array, Object
            /// 
            /// if the variant type is Array then you may wish to enquire
            /// further using array_element_type()
            public string type()
            {
                if (spi_Variant_type(self, out string vt) != 0)
                {
                    throw spi.ErrorToException();
                }
                return vt;
            }

            public string value_type
            {
                get
                {
                    return type();
                }
            }

            /// returns the common element type if the Variant is an array
            /// this will returned UNDEFINED if the Variant is not an array
            /// or if the Variant array does not have a common element type
            public string array_element_type
            {
                get
                {
                    if (spi_Variant_array_element_type(self, out string vt) != 0)
                    {
                        throw spi.ErrorToException();
                    }
                    return vt;
                }
            }

            public int[] array_dimensions
            {
                get
                {
                    if (spi_Variant_array_num_dimensions(self, out int ND) != 0)
                    {
                        throw ErrorToException();
                    }
                    int[] dimensions = new int[ND];
                    if (spi_Variant_array_dimensions(self, ND, dimensions) != 0)
                    {
                        throw spi.ErrorToException();
                    }
                    return dimensions;
                }
            }

            public System.DateTime scalar_date
            {
                get
                {
                    if (spi_Variant_DateTime(self, out double dt) != 0)
                        throw ErrorToException();

                    return DateTimeFromCDateTime(dt);
                }
            }

            public System.String scalar_string
            {
                get
                {
                    if (spi_Variant_String(self, out string str) != 0)
                        throw ErrorToException();

                    return str;
                }
            }

            public double scalar_double
            {
                get
                {
                    if (spi_Variant_Double(self, out double d) != 0)
                        throw ErrorToException();

                    return d;
                }
            }

            public int scalar_int
            {
                get
                {
                    if (spi_Variant_Int(self, out int i) != 0)
                        throw ErrorToException();

                    return i;
                }
            }

            public bool scalar_bool
            {
                get
                {
                    if (spi_Variant_Bool(self, out bool b) != 0)
                        throw ErrorToException();

                    return b;
                }
            }

            public SpiObject scalar_object
            {
                get
                {
                    if (spi_Variant_Object(self, out IntPtr o) != 0)
                        throw ErrorToException();

                    return SpiObject.Wrap(o);
                }
            }

            static public implicit operator System.DateTime(SpiVariant var)
            {
                if (spi_Variant_DateTime(var.self, out double dt) != 0)
                    throw ErrorToException();

                return DateTimeFromCDateTime(dt);
            }

            static public implicit operator System.String(SpiVariant var)
            {
                if (spi_Variant_String(var.self, out string str) != 0)
                    throw ErrorToException();

                return str;
            }

            static public implicit operator double(SpiVariant var)
            {
                if (spi_Variant_Double(var.self, out double d) != 0)
                    throw ErrorToException();

                return d;
            }

            static public implicit operator int(SpiVariant var)
            {
                if (spi_Variant_Int(var.self, out int i) != 0)
                    throw ErrorToException();

                return i;
            }

            static public implicit operator bool(SpiVariant var)
            {
                if (spi_Variant_Bool(var.self, out bool b) != 0)
                    throw ErrorToException();

                return b;
            }

            static public implicit operator SpiObject?(SpiVariant var)
            {
                if (spi_Variant_Object(var.self, out IntPtr o) != 0)
                    throw ErrorToException();

                return SpiObject.Wrap(o);
            }

            static public implicit operator System.DateTime[](SpiVariant var)
            {
                if (spi_Variant_DateTime_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                using PointerHandle h = spi.DateTimeVectorToHandle(dt);
                return spi.DateTimeVectorToArray(h);
            }

            static public implicit operator System.String[](SpiVariant var)
            {
                if (spi_Variant_String_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                using PointerHandle h = spi.StringVectorToHandle(dt);
                return spi.StringVectorToArray(h);
            }

            static public implicit operator double[](SpiVariant var)
            {
                if (spi_Variant_Double_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                using PointerHandle h = spi.DoubleVectorToHandle(dt);
                return spi.DoubleVectorToArray(h);
            }

            static public implicit operator int[](SpiVariant var)
            {
                if (spi_Variant_Int_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                using PointerHandle h = spi.IntVectorToHandle(dt);
                return spi.IntVectorToArray(h);
            }

            static public implicit operator bool[](SpiVariant var)
            {
                if (spi_Variant_Bool_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                using PointerHandle h = spi.BoolVectorToHandle(dt);
                return spi.BoolVectorToArray(h);
            }

            static public implicit operator SpiObject?[](SpiVariant var)
            {
                if (spi_Variant_Object_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                using PointerHandle h = spi.SpiObjectVectorToHandle(dt);
                return spi.SpiObjectVectorToArray(h);
            }

            // note that self = spi::Variant*
            private IntPtr self = IntPtr.Zero;
        }

        public static PointerHandle SpiVariantVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Variant_Vector_delete);
        }

        public static SpiVariant[] SpiVariantVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Variant_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            SpiVariant[] array = new SpiVariant[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Variant_Vector_item(v, i, out IntPtr item) != 0)
                {
                    throw ErrorToException();
                }
                array[i] = new SpiVariant(item);
            }

            return array;
        }

        public static PointerHandle SpiVariantVectorFromArray(SpiVariant[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Variant_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Variant_Vector_delete);

            if (array is not null)
            {
                for (int i = 0; i < size; ++i)
                {
                    if (spi_Variant_Vector_set_item(v, i, SpiVariant.get_inner(array[i])) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        public static PointerHandle SpiVariantMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Variant_Matrix_delete);
        }

        public static SpiVariant[,] SpiVariantMatrixToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            if (spi_Variant_Matrix_size(v, out int nr, out int nc) != 0)
            {
                throw ErrorToException();
            }

            SpiVariant[,] array = new SpiVariant[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Variant_Matrix_item(v, i, j, out IntPtr item) != 0)
                    {
                        throw ErrorToException();
                    }
                    array[i,j] = new SpiVariant(item);
                }
            }

            return array;
        }

        public static PointerHandle SpiVariantMatrixFromArray(SpiVariant[,]? array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr v = spi_Variant_Matrix_new(nr,nc);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Variant_Matrix_delete);

            if (array is not null)
            {
                for (int i = 0; i < nr; ++i)
                {
                    for (int j = 0; j < nc; ++j)
                    {
                        if (spi_Variant_Matrix_set_item(v, i, j, SpiVariant.get_inner(array[i, j])) != 0)
                        {
                            throw ErrorToException();
                        }
                    }
                }
            }

            return h;
        }

        // TBA: should we allow these classes which wrap a reference counted pointer
        // to have one constructor which takes ownership and another which increments
        // the reference count?

        public class SpiObject : IDisposable
        {
            protected SpiObject()
            {}

            ~SpiObject()
            {
                Dispose(false);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (self != IntPtr.Zero)
                {
                    spi_Object_delete(self);
                    self = IntPtr.Zero;
                }
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            public delegate SpiObject _class_wrapper(IntPtr self);
            public static System.Collections.Generic.Dictionary<string, _class_wrapper> _class_wrappers;

            static SpiObject()
            {
                _class_wrappers = new System.Collections.Generic.Dictionary<string, _class_wrapper>();
                _class_wrappers.Add("Map", SpiMap.Wrap);
            }

            public static void _register_class_wrapper(string name, _class_wrapper wrapper)
            {
                _class_wrappers.Add(name, wrapper);
            }

            public static SpiObject? WrapNullable(IntPtr self)
            {
                if (self == IntPtr.Zero)
                    return null;

                return SpiObject.Wrap(self);
            }

            public static SpiObject Wrap(IntPtr self)
            {
                if (self == IntPtr.Zero)
                    throw new Exception("SpiObject.Wrap cannot wrap a null pointer");

                if (spi_Object_get_class_name(self, out string className) != 0)
                    throw spi.ErrorToException();

                try
                {
                    _class_wrapper wrapper = _class_wrappers[className];
                    return wrapper(self);
                }
                catch
                {
                    SpiObject obj = new SpiObject();
                    obj.set_inner(self);
                    return obj;
                }
            }

            public string object_id
            {
                get
                {
                    if (self == IntPtr.Zero)
                        throw new Exception("Cannot return object_id for a null pointer");

                    if (spi_Object_get_object_id(self, out string objectId) != 0)
                        throw spi.ErrorToException();

                    return objectId;
                }
            }

            public string? typename
            {
                get
                {
                    if (self == IntPtr.Zero)
                        return null;

                    if (spi_Object_get_class_name(self, out string typename) != 0)
                        throw spi.ErrorToException();

                    return typename;
                }
            }

            /// <summary>
            /// Serializes the object to string using the given format
            /// </summary>
            /// <param name="format">
            /// Optional. Object string format.
            /// </param>
            /// <param name="options">
            /// Optional. Extra options for the given format.
            /// </param>
            /// <returns></returns>
            public System.String to_string(System.String format = "", System.String options = "")
            {
                if (spi_Object_to_string(self, format, options, out string str) != 0)
                {
                    throw ErrorToException();
                }
                return str;
            }

            /// <summary>
            /// De-serializes the string to an spi.Object
            /// </summary>
            /// <param name="str">
            /// The serialized object string, as created by the to_string method.
            /// </param>
            /// <returns></returns>
            public static SpiObject from_string(System.String str)
            {
                IntPtr inner = spi_Object_from_string(str);
                if (inner == IntPtr.Zero)
                    throw spi.ErrorToException();
                return SpiObject.Wrap(inner);
            }

            /// <summary>
            /// Serializes the object to file using the given format.
            /// </summary>
            /// <param name="filename">
            /// Name of the file to be written.
            /// </param>
            /// <param name="format">
            /// Optional. Object file format.
            /// </param>
            /// <param name="options">
            /// Optional. extra options for the given format.
            /// </param>
            /// <returns></returns>
            public System.String to_file(System.String filename,
                System.String format = "", System.String options = "")
            {
                if (spi_Object_to_file(self, filename, format, options) != 0)
                {
                    throw ErrorToException();
                }
                return filename;
            }

            /// <summary>
            /// De-serializes the object stored on file to an spi.Object
            /// </summary>
            /// <param name="filename">
            /// The filename containing the serialized object, e.g. as created by the to_file method.
            /// </param>
            /// <returns></returns>
            public static SpiObject from_file(System.String filename)
            {
                IntPtr inner = spi_Object_from_file(filename);
                if (inner == IntPtr.Zero)
                    throw spi.ErrorToException();
                return SpiObject.Wrap(inner);
            }

            /// <summary>
            /// Equivalent to object_get in Excel.
            /// </summary>
            /// <param name="name"></param>
            /// <returns>
            /// Variant value - you can then use implicit type conversions to
            /// convert to the actual type, e.g. int numItems = o.get_value("numItems")
            /// </returns>
            public SpiVariant get_value(System.String name)
            {
                IntPtr value = spi_Object_get_value(self, name);
                if (value == IntPtr.Zero)
                    throw ErrorToException();

                return new SpiVariant(value);
            }

            protected void set_inner(IntPtr self)
            {
                // in_self should have been returned via P/INVOKE with reference
                // count incremented by one
                //
                // hence if it is the same as self then we can safely delete self
                //
                // note that the only time we would change self for an existing
                // object is after we have changed one of its attributes
                if (self == IntPtr.Zero)
                    throw new Exception("Cannot set_inner pointer to NULL");

                if (this.self != IntPtr.Zero)
                    spi_Object_delete(this.self);

                this.self = self;
            }

            public static IntPtr get_inner(SpiObject? o)
            {
                if (o == null)
                    return IntPtr.Zero;

                return o.self;
            }

            // note that self = spi::Object* or sub-class thereof
            protected IntPtr self = IntPtr.Zero;
        }

        // functions dealing with string object handles
        public static System.String ObjectHandleSave(
            System.String baseName,
            SpiObject obj,
            bool noCount = false)
        {
            if (spi_Object_handle_save(baseName, SpiObject.get_inner(obj), noCount, out string handle) != 0)
            {
                throw ErrorToException();
            }
            return handle;
        }

        public static SpiObject ObjectHandleFind(
            System.String handle)
        {
            if (spi_Object_handle_find(handle, out IntPtr obj) != 0)
            {
                throw ErrorToException();
            }
            return SpiObject.Wrap(obj);
        }

        public static int ObjectHandleFreeAll()
        {
            if (spi_Object_handle_free_all(out int count) != 0)
            {
                throw ErrorToException();
            }
            return count;
        }

        public static int ObjectHandleCount(
            System.String className)
        {
            if (spi_Object_handle_count(className, out int count) != 0)
            {
                throw ErrorToException();
            }
            return count;
        }

        public static void ObjectHandleFree(
            System.String handle)
        {
            if (spi_Object_handle_free(handle) != 0)
            {
                throw ErrorToException();
            }
        }

        public static string[] ObjectHandleList(
            System.String baseName = "",
            System.String className = "")
        {
            if (spi_Object_handle_list(baseName, className, out IntPtr handles) != 0)
            {
                throw ErrorToException();
            }
            return spi.StringVectorToArray(spi.StringVectorToHandle(handles));
        }

        public static string ObjectHandleClassName(
            string handle)
        {
            if (spi_Object_handle_class_name(handle, out string className) != 0)
            {
                throw ErrorToException();
            }
            return className;
        }

        public static PointerHandle SpiObjectVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Object_Vector_delete);
        }

        public static SpiObject[] SpiObjectVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Object_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            SpiObject[] array = new SpiObject[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Object_Vector_item(v, i, out IntPtr item) != 0)
                {
                    throw ErrorToException();
                }
                array[i] = SpiObject.Wrap(item); // will detect null items
            }

            return array;
        }

        public static PointerHandle SpiObjectVectorFromArray(SpiObject[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Object_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Object_Vector_delete);

            if (array is not null)
            {
                for (int i = 0; i < size; ++i)
                {
                    IntPtr item = SpiObject.get_inner(array[i]);
                    if (item == IntPtr.Zero)
                    {
                        throw new Exception("Null pointer in input array");
                    }
                    if (spi_Object_Vector_set_item(v, i, item) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        public static SpiObject?[] SpiObjectNullableVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Object_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            SpiObject?[] array = new SpiObject[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Object_Vector_item(v, i, out IntPtr item) != 0)
                {
                    throw ErrorToException();
                }
                array[i] = SpiObject.WrapNullable(item);
            }

            return array;
        }

        public static PointerHandle SpiObjectNullableVectorFromArray(SpiObject[]? array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Object_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Object_Vector_delete);

            if (array is not null)
            {
                for (int i = 0; i < size; ++i)
                {
                    if (spi_Object_Vector_set_item(v, i, SpiObject.get_inner(array[i])) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        public static PointerHandle SpiObjectMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Object_Matrix_delete);
        }

        public static SpiObject?[,] SpiObjectMatrixToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            if (spi_Object_Matrix_size(v, out int nr, out int nc) != 0)
            {
                throw ErrorToException();
            }

            SpiObject?[,] array = new SpiObject[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Object_Matrix_item(v, i, j, out IntPtr item) != 0)
                    {
                        throw ErrorToException();
                    }
                    array[i, j] = SpiObject.Wrap(item);
                }
            }

            return array;
        }

        public static PointerHandle SpiObjectMatrixFromArray(SpiObject[,]? array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr v = spi_Object_Matrix_new(nr, nc);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Object_Matrix_delete);

            if (array is not null)
            {
                for (int i = 0; i < nr; ++i)
                {
                    for (int j = 0; j < nc; ++j)
                    {
                        if (spi_Object_Matrix_set_item(v, i, j, SpiObject.get_inner(array[i, j])) != 0)
                        {
                            throw ErrorToException();
                        }
                    }
                }
            }

            return h;
        }

        /* Map functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Map_dynamic_cast(IntPtr o);

        public class SpiMap : SpiObject
        {
            protected SpiMap() { }

            // self should be spi::MapObject*
            public new static SpiMap Wrap(IntPtr self)
            {
                if (self == IntPtr.Zero)
                    throw new Exception("Attempt to wrap null pointer");

                self = spi_Map_dynamic_cast(self);
                if (self == IntPtr.Zero)
                    throw new Exception("self is not an instance of Map");

                SpiMap obj = new SpiMap();
                obj.set_inner(self);
                return obj;
            }

        }
    }
}
