def pad_sequence(sequences: List[Tensor],
    batch_first: bool=False,
    padding_value: float=0.) -> Tensor:
  _0 = torch.pad_sequence(sequences, batch_first, padding_value)
  return _0
