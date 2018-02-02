// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Delegated stream interfaces.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream.h>

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Delegated input stream implementation.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class DelegatedInputStream : public InputStream
    {
    protected:
        InputStream& m_InputStream;

    public:

        /*!
         * Initializes delegated input stream.
         * @param inputStream The stream that implements current delegated stream.
         */
        GDINL explicit DelegatedInputStream(InputStream& inputStream)
            : m_InputStream(inputStream)
        {
        }

    public:

        // ------------------------------------------------------------------------------------------
        // BaseStream interface.
        // ------------------------------------------------------------------------------------------

        /*!
         * Returns true if this stream is valid and ready for I/O operations.
         */
        GDINT virtual bool IsValid() const override
        {
            return m_InputStream.IsValid();
        }

        // ------------------------------------------------------------------------------------------
        // InputStream interface.
        // ------------------------------------------------------------------------------------------

        /*!
         * Returns current position in stream or -1 on error.
         */
        GDINT virtual SizeTp GetPosition() const override
        {
            return m_InputStream.GetPosition();
        }

        /*!
         * Returns size of data that stream handles or -1 on error.
         */
        GDINT virtual SizeTp GetLength() const override
        {
            return m_InputStream.GetLength();
        }

        /*!
         * Closes this stream and releases all resources associated with this stream.
         */
        GDINT virtual void Close() override
        {
            m_InputStream.Close();
        }

        /*!
         * Reposition this stream to new specified position.
         *
         * @param offset The offset in bytes from specified origin.
         * @param origin Defines origin from which point make offset.
         *
         * @returns New position in file, -1 on error.
         */
        GDINT virtual SizeTp Seek(PtrDiffTp const offset, SeekOrigin const origin = SeekOrigin::Current) override
        {
            return m_InputStream.Seek(offset, origin);
        }

        /*!
         * Reads next byte from input stream.
         * @returns Read byte from stream, or -1 on the error.
         */
        GDINT virtual Int16 Read() override
        {
            return m_InputStream.Read();
        }

        /*!
         * Reads several elements from input stream.
         *
         * @param array Output memory to which data would be written.
         * @param size Length of the element in bytes,
         * @param count Amount of the elements.
         *
         * @returns Total amount of elements read.
         */
        GDINT virtual SizeTp Read(Handle const array, SizeTp const size, SizeTp const count) override final
        {
            return m_InputStream.Read(array, size, count);
        }

    };	// class DelegatedInputStream

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Delegated input stream implementation.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class DelegatedOutputStream : public OutputStream
    {
    protected:
        OutputStream& m_OutputStream;

    public:

        /*!
         * Initializes delegated input stream.
         * @param inputStream The stream that implements current delegated stream.
         */
        GDINL explicit DelegatedOutputStream(OutputStream& outputStream)
            : m_OutputStream(outputStream)
        {
        }

        // ------------------------------------------------------------------------------------------
        // BaseStream interface.
        // ------------------------------------------------------------------------------------------

        /*!
         * Returns true if this stream is valid and ready for I/O operations.
         */
        GDINT virtual bool IsValid() const override
        {
            return m_OutputStream.IsValid();
        }

        // ------------------------------------------------------------------------------------------
        // InputStream interface.
        // ------------------------------------------------------------------------------------------

        /*!
         * Closes this stream and releases all resources associated with this stream.
         */
        GDINT virtual void Close() override
        {
            m_OutputStream.Close();
        }

        /*!
         * Writes all unsaved to the resource.
         */
        GDINT virtual void Flush() override
        {
            m_OutputStream.Flush();
        }

        /*!
         * Writes a byte into output.
         *
         * @param byte Value that would be written to output.
         * @returns True if operation succeeded.
         */
        GDAPI virtual bool Write(Byte const byte) override final
        {
            return m_OutputStream.Write(byte);
        }

        /*!
         * Writes several elements to output.
         *
         * @param array Input elements that would be written.
         * @param size Length of the element in bytes,
         * @param count Amount of the elements.
         *
         * @returns Total amount of elements written.
         */
        GDINT virtual SizeTp Write(CHandle const array, SizeTp const size, SizeTp const count) override final
        {
            return m_OutputStream.Write(array, size, count);
        }

    };	// class DelegatedOutputStream

GD_NAMESPACE_END
