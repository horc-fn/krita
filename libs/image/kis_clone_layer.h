/*
 *  Copyright (c) 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_CLONE_LAYER_H_
#define KIS_CLONE_LAYER_H_

#include <QObject>
#include "kis_types.h"
#include "kis_layer.h"

#include <kritaimage_export.h>
#include "kis_clone_info.h"

class KisNodeVisitor;

enum CopyLayerType {
    COPY_PROJECTION,
    COPY_ORIGINAL
};


/**
 * A copy layer adds the contents of another layer in another place in
 * the layer stack. It is possible to add more effect masks to the
 * copy. You can either copy the original data or the projection data
 * produced by the original layer + original effect masks. There is no
 * physical copy of the data; if the original changes, the copy
 * changes too. The copy layer can be positioned differently from the
 * original layer.
 **/
class KRITAIMAGE_EXPORT KisCloneLayer : public KisLayer
{

    Q_OBJECT

public:

    KisCloneLayer(KisLayerSP from, KisImageWSP image, const QString &name, quint8 opacity);
    KisCloneLayer(const KisCloneLayer& rhs);
    ~KisCloneLayer() override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisCloneLayer(*this));
    }

    /**
     * When the source layer of the clone is removed from the stack
     * we should substitute the clone with a usual paint layer,
     * because the source might become unreachable quite soon. This
     * method builds a paint layer representation of this clone.
     */
    KisLayerSP reincarnateAsPaintLayer() const;

    bool allowAsChild(KisNodeSP) const override;

    KisPaintDeviceSP original() const override;
    KisPaintDeviceSP paintDevice() const override;
    bool needProjection() const override;

    QIcon icon() const override;
    KisBaseNode::PropertyList sectionModelProperties() const override;

    qint32 x() const override;
    qint32 y() const override;

    void setX(qint32) override;
    void setY(qint32) override;

    /// Returns an approximation of where the bounds on actual data are in this layer
    QRect extent() const override;

    /// Returns the exact bounds of where the actual data resides in this layer
    QRect exactBounds() const override;

    bool accept(KisNodeVisitor &) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    /**
     * Used when loading: loading is done in two passes, and the copy
     * from layer is set when all layers have been created, not during
     * loading.
     */
    void setCopyFromInfo(KisCloneInfo info);
    KisCloneInfo copyFromInfo() const;

    void setCopyFrom(KisLayerSP layer);
    KisLayerSP copyFrom() const;

    void setCopyType(CopyLayerType type);
    CopyLayerType copyType() const;

    /**
     * This function is called by the original to notify
     * us that it is dirty
     */
    void setDirtyOriginal(const QRect &rect);

    QRect needRectOnSourceForMasks(const QRect &rc) const;

    void syncLodCache() override;

protected:
    // override from KisNode
    QRect accessRect(const QRect &rect, PositionToFilthy pos) const override;

    // override from KisLayer
    void copyOriginalToProjection(const KisPaintDeviceSP original,
                                  KisPaintDeviceSP projection,
                                  const QRect& rect) const override;

    void notifyParentVisibilityChanged(bool value) override;
    QRect outgoingChangeRect(const QRect &rect) const override;
private:

    struct Private;
    Private * const m_d;

};

#endif // KIS_CLONE_LAYER_H_

