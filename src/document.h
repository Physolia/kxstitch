/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef DOCUMENT_H
#define DOCUMENT_H


#include <QColor>
#include <QIcon>
#include <QImage>
#include <QList>
#include <QMap>
#include <QRect>
#include <QStack>
#include <QString>
#include <QUndoStack>
#include <QVector>

#include <KUrl>

#include "stitch.h"


class Floss;
class DocumentFloss;
class Editor;
class Palette;
class Preview;
class SchemeManager;
class BackgroundImage;


/**
	V10 Document consists of
		KXStitch					QString
		file format version			quint16
		property list				QVariantMap
		floss count					quint32
			floss key				quint32
			floss name				QString
			floss symbol			QChar
			strands / stitch		quint8
			strands / backstitch	quint8
		used flosses count			quint32
			floss key				quint32
			number of stitches		quint32
		stitches count				quint32
			stitch cell index		quint32
			stitches in the queue	quint8
				stitch type			quint8
				floss index			quint32
		backstitch count			quint32
			start					QPoint
			end						QPoint
			floss key				quint32
		knot count					quint32
			position				QPoint
			floss key				quint32
		background image list		QList
			url						KUrl
			location rectangle		QRect
			visible flag			bool
			image					QImage
			image icon				QIcon
*/


class Document
{
	public:
		Document();
		~Document();

		void initialiseNew();

		unsigned  width() const;
		unsigned  height() const;
		void resizeDocument(int, int);

		void setEditor(Editor *);
		void setPalette(Palette *);
		void setPreview(Preview *);

		bool loadURL(const KUrl &);
		KUrl URL() const;
		void setURL(const KUrl &);
		bool isModified() const;
		bool saveDocument();
		QVariant property(const QString &) const;
		void setProperty(const QString &, const QVariant &);
		StitchQueue *stitchAt(const QPoint &) const;
		StitchQueue *replaceStitchAt(const QPoint &, StitchQueue *);
		StitchQueue *takeStitchAt(const QPoint &);

		bool addStitch(const QPoint &, Stitch::Type, int);
		bool deleteStitch(const QPoint &, Stitch::Type, int);
		bool addBackstitch(const QPoint &, const QPoint &, int);
		bool deleteBackstitch(const QPoint &, const QPoint &, int);
		bool addFrenchKnot(const QPoint &, int);
		bool deleteFrenchKnot(const QPoint &, int);

		void clearUnusedColors();
		QMap<int, DocumentFloss *> &palette();
		void addFloss(int, DocumentFloss *);
		DocumentFloss *removeFloss(int);
		DocumentFloss *changeFloss(int, DocumentFloss *);
		const Floss *floss(int) const;
		int currentFlossIndex() const;
		void setCurrentFlossIndex(int);
		bool paletteManager();
		SchemeManager *schemeManager();

		QListIterator<BackgroundImage *> backgroundImages() const;
		QListIterator<Backstitch *> backstitches() const;
		QListIterator<Knot *> knots() const;
		void addBackgroundImage(const KUrl &, const QRect &);
		void addBackgroundImage(BackgroundImage *);
		BackgroundImage *removeBackgroundImage(const QString &);
		BackgroundImage *removeBackgroundImage(BackgroundImage *);
		QRect fitBackgroundImage(const QString &, const QRect &);
		bool showBackgroundImage(const QString &, bool);
		QRect extents();
		void movePattern(int, int, int);

		QUndoStack &undoStack();

	private:
		Editor		*m_editorWidget;
		Palette		*m_paletteWidget;
		Preview		*m_previewWidget;

		unsigned int canvasIndex(const QPoint &) const;
		unsigned int canvasIndex(int, int) const;
		bool validateCell(const QPoint &) const;
		bool validateSnap(const QPoint &) const;
		void updateUsedFlosses();

		// Document properties
		QVariantMap				m_properties;
		KUrl					m_documentURL;
		QList<BackgroundImage *>	m_backgroundImages;

		// Palette properties
		SchemeManager			*m_schemeManager;
		QMap<int, DocumentFloss *>	m_palette;

		// Canvas properties
		QMap<unsigned int, StitchQueue *>	m_canvasStitches;
		QMap<int, int>			m_usedFlosses;
		QList<Knot *>			m_canvasKnots;
		QList<Backstitch *>		m_canvasBackstitches;

		QUndoStack				m_undoStack;
};


#endif
