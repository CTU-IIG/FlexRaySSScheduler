/// \file Message.h Data structure for the message [Two Stage scheduler]

#ifndef MESSAGE_H_
#define MESSAGE_H_

///Data structure for the message
struct Message {
    std::vector<int> signals; ///< Vector of the signal transmitting by the message
    int period; ///< The period of the message
    int deadline; ///< The deadline of the message
    int releaseTime; ///< The release date of the message
    int length; ///< Payload of the message
    int slot; ///< Slot ID in which the message is scheduled
    int startCycle; ///< Cycle ID in which the occurence of the message is scheduled
    int offsetInFrame; ///< Offset of the message in the frame
};

#endif /* MESSAGE_H_ */ 