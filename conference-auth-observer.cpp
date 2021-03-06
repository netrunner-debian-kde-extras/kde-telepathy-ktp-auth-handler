/*
 * Copyright (C) 2012 Rohan Garg <rohangarg@kubuntu.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "conference-auth-observer.h"

#include "conference-auth-op.h"

#include <KTp/telepathy-handler-application.h>

#include <QDBusConnection>
#include <QDebug>

#include <TelepathyQt/Channel>
#include <TelepathyQt/ChannelDispatchOperation>
#include <TelepathyQt/MethodInvocationContext>


ConferenceAuthObserver::ConferenceAuthObserver(const Tp::ChannelClassSpecList &channelFilter)
    : Tp::AbstractClientObserver(channelFilter)
{
}

ConferenceAuthObserver::~ConferenceAuthObserver()
{
}

void ConferenceAuthObserver::observeChannels(const Tp::MethodInvocationContextPtr<> &context,
                                             const Tp::AccountPtr &account,
                                             const Tp::ConnectionPtr &connection,
                                             const QList<Tp::ChannelPtr> &channels,
                                             const Tp::ChannelDispatchOperationPtr &dispatchOperation,
                                             const QList<Tp::ChannelRequestPtr> &requestsSatisfied,
                                             const Tp::AbstractClientObserver::ObserverInfo &observerInfo)
{
    Q_UNUSED(connection);
    Q_UNUSED(requestsSatisfied);
    Q_UNUSED(observerInfo);
    Q_UNUSED(dispatchOperation)

    Q_FOREACH (Tp::ChannelPtr channel, channels) {
        if (!channel->hasInterface(TP_QT_IFACE_CHANNEL_INTERFACE_PASSWORD)) {
            qDebug() << "Channel does not have password interface, exiting ...";
            continue;
        }

        KTp::TelepathyHandlerApplication::newJob();
        ConferenceAuthOp *auth = new ConferenceAuthOp(
                    account, channel);
        connect(auth,
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(onAuthFinished(Tp::PendingOperation*)));
    }

    context->setFinished();

}

void ConferenceAuthObserver::onAuthFinished(Tp::PendingOperation *op)
{
    if (op->isError()) {
        qWarning() << "Error in conference room auth:" << op->errorName() << "-" << op->errorMessage();
    }

    KTp::TelepathyHandlerApplication::jobFinished();
}
