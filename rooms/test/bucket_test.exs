defmodule Rooms.BucketTest do
  use ExUnit.Case, async: true

  setup do
    {:ok, bucket} = Rooms.Bucket.start_link([])
    %{bucket: bucket}
  end

  test "stores values by key", %{bucket: bucket} do
    assert Rooms.Bucket.get(bucket, "milk") == nil
    Rooms.Bucket.put(bucket, "milk", 3)
    assert Rooms.Bucket.get(bucket, "milk") == 3
  end

  test "can remove values by key", %{bucket: bucket} do
    Rooms.Bucket.put(bucket, "milk", 3)
    Rooms.Bucket.delete(bucket, "milk")
    assert Rooms.Bucket.get(bucket, "milk") == nil
  end

  test "are temporary workers" do
    assert Supervisor.child_spec(Rooms.Bucket, []).restart == :temporary
  end
end
